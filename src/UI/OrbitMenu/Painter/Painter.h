#pragma once

#include "Config/Settings.h"
#include "UI/Graphics/GpuTexture.h"
#include "UI/OrbitMenu/Animation.h"
#include "UI/OrbitMenu/Content.h"
#include "UI/OrbitMenu/Curve.h"
#include "UI/OrbitMenu/IconCache.h"
#include "UI/OrbitMenu/Layout.h"
#include "UI/Skins/SkinImages.h"

namespace Orbit::UI::OrbitMenu
{
	struct Scene
	{
		const Config::Settings &settings;
		const MenuInput &input;
		const Motion &motion;
		const Content &content;
		const IconCache &icons;
		const Skins::SkinImages &skin;
	};

	class Painter
	{
	public:
		Painter(ImDrawList &a_drawList, ImFont &a_font, ImVec2 a_display, const Scene &a_scene);
		void Paint() const;

	private:
		struct SlotKey
		{
			ImVec2 at;
			float size;
			const TextLine &text;
			ImVec4 color;
			float alpha;
		};

		static constexpr std::size_t kEdgeSegments = 8;
		static constexpr float kSlotInnerRingInset = 6.0f;
		static constexpr float kHidden = 0.01f;
		static constexpr float kNoneLeftAlpha = 0.45f;
		float Px(float a_base) const;
		float TextPx(std::int32_t a_size) const;
		float SlotRadius(int a_slot) const;
		float SlotAlpha(int a_slot) const;
		ImVec2 NameAt(int a_slot) const;
		void Bend(int a_firstVertex, int a_controlsHintVertex) const;
		void TextAt(float a_size, ImVec2 a_topLeft, ImVec4 a_color, std::string_view a_text) const;
		void Text(float a_size, ImVec2 a_topCenter, ImVec4 a_color, const TextLine &a_line) const;
		void TextCentered(float a_size, ImVec2 a_center, ImVec4 a_color, const TextLine &a_line) const;
		void Line(ImVec2 a_from, ImVec2 a_to, ImU32 a_color, float a_thickness) const;
		void Image(const Graphics::CachedImage &a_image, ImVec2 a_center, float a_size, ImU32 a_color) const;
		void RotatedImage(const Graphics::CachedImage &a_image, ImVec2 a_center, ImVec2 a_up, ImVec2 a_right, ImU32 a_color) const;
		void DrawBackdrop() const;
		void DrawItemDescriptionBackground() const;
		void DrawItemDescriptionIcon() const;
		void DrawItemDescription() const;
		void DrawPointer() const;
		void DrawSlotGlow(int a_slot) const;
		void DrawSlotShape(int a_slot) const;
		void DrawSkinSlot(int a_slot) const;
		void DrawSlotIcons() const;
		template <class Visit>
		void ForEachSlotKey(const Visit &a_visit) const;
		void DrawSlotLabels(int a_slot) const;
		void DrawSlotKeys() const;
		void DrawHands() const;
		void DrawControlsHint() const;
		void DrawPageIndicator() const;
		ImDrawList &_drawList;
		ImFont &_font;
		ImVec2 _display;
		const Scene &_scene;
		const Palette &_colors;
		Layout _layout;
		Curve _curve;
		float _alpha;
		int _strongestSlot;
		float _strength;
		int _descriptionSlot;
		float _descriptionStrength;
	};
}
