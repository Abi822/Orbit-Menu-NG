#pragma once

namespace Orbit::Config::SettingsMenu
{
	class MenuFrameworkBase
	{
	public:
		bool SaveRequested() const { return _save; }
		void RequestSave() { _save = true; }

	protected:
		static const char *Label(const char *a_label);
		static void Tooltip(const char *a_text);
		void Slider(const char *a_label, float &a_value, float a_min, float a_max, const char *a_format, const char *a_tooltip = nullptr);
		void Slider(const char *a_label, std::int32_t &a_value, std::int32_t a_min, std::int32_t a_max, const char *a_tooltip = nullptr);
		void Number(const char *a_label, std::int32_t &a_value, std::int32_t a_min, std::int32_t a_max, std::int32_t a_step = 1, const char *a_tooltip = nullptr);
		void Check(const char *a_label, bool &a_value, const char *a_tooltip = nullptr);
		void Color(const char *a_label, std::uint32_t &a_argb);
		bool _save = false;
	};
}
