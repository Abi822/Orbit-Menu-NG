#include "UI/Graphics/SwfIcon.h"

namespace Orbit::UI::Graphics
{
	namespace
	{
		constexpr std::string_view kSkyUIItemIcons = "skyui/icons_item_psychosteve.swf";

		template <class T>
		RE::GPtr<T> Adopt(T *a_object)
		{
			RE::GPtr<T> pointer{a_object};
			if (a_object)
			{
				a_object->Release();
			}
			return pointer;
		}

		float NumberMember(const RE::GFxValue &a_object, const char *a_name)
		{
			RE::GFxValue value;
			return a_object.GetMember(a_name, &value) && value.IsNumber() ? static_cast<float>(value.GetNumber()) : 0.0f;
		}

		RE::GRenderer::Cxform TintTransform(std::uint32_t a_tint)
		{
			using Cxform = RE::GRenderer::Cxform;
			Cxform transform;
			for (const auto channel : {Cxform::kR, Cxform::kG, Cxform::kB})
			{
				transform.matrix[channel][Cxform::kMult] = 0.0f;
				transform.matrix[channel][Cxform::kAdd] = static_cast<float>((a_tint >> (16 - 8 * channel)) & 0xFF);
			}
			return transform;
		}

		void TintChildren(const RE::GFxValue &a_clip, const RE::GRenderer::Cxform &a_transform, int a_depth = 0)
		{
			constexpr int kMaxDepth = 8;
			if (a_depth >= kMaxDepth)
			{
				return;
			}
			std::vector<RE::GFxValue> children;
			a_clip.VisitMembers([&children](const char *, const RE::GFxValue &a_value)
								{
				if (a_value.IsDisplayObject())
				{
					children.push_back(a_value);
				} });
			for (auto &child : children)
			{
				child.SetCxform(a_transform);
				TintChildren(child, a_transform, a_depth + 1);
			}
		}

		class SwfFiles
		{
		public:
			static SwfFiles &Get()
			{
				static auto *files = new SwfFiles;
				return *files;
			}

			RE::GFxMovieDef *Find(const std::string &a_source)
			{
				if (const auto it = _files.find(a_source); it != _files.end())
				{
					return it->second.get();
				}

				std::string path = "Interface/" + a_source;
				std::ranges::replace(path, '\\', '/');

				RE::GPtr<RE::GFxMovieDef> file;
				auto *manager = RE::BSScaleformManager::GetSingleton();
				if (manager && manager->loader && RE::BSScaleformManager::FileExists(path.c_str()))
				{
					const auto flags = static_cast<RE::GFxLoader::LoadConstants>(RE::GFxLoader::kLoadKeepBindData | RE::GFxLoader::kLoadWaitCompletion);
					file = Adopt(manager->loader->CreateMovie(path.c_str(), flags));
				}
				if (file)
				{
					logger::info("Swf Icon: Icon file {} loaded", path);
				}
				else
				{
					logger::warn("Swf Icon: Icon file {} could not be loaded! Slots using it will show no icon!", path);
				}
				return _files.emplace(a_source, std::move(file)).first->second.get();
			}

			void ReportMissing(const std::string &a_source, std::string_view a_label)
			{
				if (_missing.insert(a_source + '|' + std::string{a_label}).second)
				{
					logger::warn("Swf Icon: Icon {} not found in {}! Slot shows no icon!", a_label, a_source);
				}
			}

		private:
			SwfFiles() = default;

			std::map<std::string, RE::GPtr<RE::GFxMovieDef>, std::less<>> _files;
			std::set<std::string, std::less<>> _missing;
		};
	}

	SwfIcon::SwfIcon(std::string_view a_source, std::string_view a_label, std::optional<std::uint32_t> a_tint)
	{
		if (a_label.empty())
		{
			return;
		}
		auto &files = SwfFiles::Get();
		const std::string source{a_source.empty() ? kSkyUIItemIcons : a_source};
		auto *file = files.Find(source);
		if (!file)
		{
			return;
		}

		auto view = Adopt(file->CreateInstance(true, 0));
		if (!view)
		{
			return;
		}
		view->SetViewScaleMode(RE::GFxMovieView::ScaleModeType::kExactFit);
		view->SetBackgroundAlpha(0.0f);
		RE::GFxValue root;
		RE::GFxValue bounds;
		if (view->GotoLabeledFrame(std::string{a_label}.c_str()))
		{
			view->SetPlayState(RE::GFxMovie::PlayState::kStopped);
			view->Advance(0.0f);
			if (view->GetVariable(&root, "_root") && root.Invoke("getBounds", &bounds, &root, 1) && bounds.IsObject())
			{
				_content = {NumberMember(bounds, "xMin"), NumberMember(bounds, "yMin"), NumberMember(bounds, "xMax"), NumberMember(bounds, "yMax")};
				_stage = file->GetFrameRect();
			}
		}
		if (_content.right <= _content.left || _content.bottom <= _content.top || _stage.right <= _stage.left || _stage.bottom <= _stage.top)
		{
			files.ReportMissing(source, a_label);
			return;
		}
		if (a_tint)
		{
			TintChildren(root, TintTransform(*a_tint));
		}
		_view = std::move(view);
		_root = root;
	}

	void SwfIcon::Render(int a_x, int a_y, int a_size, int a_targetWidth, int a_targetHeight)
	{
		if (!_view || a_size < 1)
		{
			return;
		}
		Fit(static_cast<float>(a_size));
		_view->SetViewport(a_targetWidth, a_targetHeight, a_x, a_y, a_size, a_size);
		_view->Display();
	}

	void SwfIcon::Fit(float a_size)
	{
		const float stageWidth = _stage.right - _stage.left;
		const float stageHeight = _stage.bottom - _stage.top;
		const float contentWidth = _content.right - _content.left;
		const float contentHeight = _content.bottom - _content.top;

		const float pixels = a_size / std::max(contentWidth, contentHeight);
		const float scaleX = pixels * stageWidth / a_size;
		const float scaleY = pixels * stageHeight / a_size;
		const float x = _stage.left + (a_size - contentWidth * pixels) * 0.5f * stageWidth / a_size - _content.left * scaleX;
		const float y = _stage.top + (a_size - contentHeight * pixels) * 0.5f * stageHeight / a_size - _content.top * scaleY;

		RE::GFxValue::DisplayInfo info;
		info.SetPosition(x, y);
		info.SetScale(scaleX * 100.0, scaleY * 100.0);
		_root.SetDisplayInfo(info);
	}
}
