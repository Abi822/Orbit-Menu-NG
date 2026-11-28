#pragma once

#include "Config/Settings.h"
#include "UI/Graphics/GpuTexture.h"
#include "UI/OrbitMenu/IconCache.h"
#include "UI/OrbitMenu/MenuAnimation.h"
#include "UI/OrbitMenu/MenuContent.h"
#include "UI/OrbitMenu/MenuLayout.h"
#include "UI/OrbitMenu/OrbitMenuCurved.h"
#include "UI/Skins/SkinImages.h"

namespace Orbit::UI::OrbitMenu
{
	struct Scene
	{
		const Config::Settings &settings;
		const MenuInput &input;
		const Motion &motion;
		const MenuContent &content;
		const IconCache &icons;
		const Skins::SkinImages &skin;
	};

	class MenuPainter
	{
	public:
		MenuPainter(ImDrawList &a_drawList, ImFont &a_font, ImVec2 a_display, const Scene &a_scene);

		void Paint() const;

	private:
		struct Badge
		{
			ImVec2 at;
			float size;
			const TextLine &text;
			ImVec4 color;
			float alpha;
		};

		float Px(float a_base) const;
		float TextPx(std::int32_t a_size) const;
		float SlotRadius(int a_slot) const;
		float SlotAlpha(int a_slot) const;
		ImVec2 NameAt(int a_slot) const;
		template <class Visit>
		void ForEachBadge(const Visit &a_visit) const;
		void TextAt(float a_size, ImVec2 a_topLeft, ImVec4 a_color, std::string_view a_text) const;
		void Text(float a_size, ImVec2 a_topCenter, ImVec4 a_color, const TextLine &a_line) const;
		void TextCentered(float a_size, ImVec2 a_center, ImVec4 a_color, const TextLine &a_line) const;
		void Line(ImVec2 a_from, ImVec2 a_to, ImU32 a_color, float a_thickness) const;
		void Image(const Graphics::CachedImage &a_image, ImVec2 a_center, float a_size, ImU32 a_color) const;
		void DrawBackdrop() const;
		void DrawCentre() const;
		void DrawPointer() const;
		void DrawSlotGlow(int a_slot) const;
		void DrawSlotShape(int a_slot) const;
		void DrawSkinSlot(int a_slot) const;
		void DrawIcons() const;
		void DrawDescription() const;
		void DrawSlotLabels(int a_slot) const;
		void DrawBadges() const;
		void DrawHands() const;
		void DrawControlsHint() const;
		void DrawPageIndicator() const;
		void Bend(int a_firstVertex, int a_controlsHintVertex) const;
		ImDrawList &_drawList;
		ImFont &_font;
		ImVec2 _display;
		const Scene &_scene;
		const MenuPalette &_colors;
		MenuLayout _layout;
		OrbitMenuCurved _curve;
		float _alpha;
		int _strongestSlot;
		float _strength;
		int _descriptionSlot;
		float _descriptionStrength;
	};
}
