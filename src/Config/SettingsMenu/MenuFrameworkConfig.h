#pragma once

namespace Orbit::Config::SettingsMenu
{
	class MenuFrameworkConfig
	{
	public:
		static void __stdcall RenderSettings();
		static void __stdcall RenderDisplay();
		static void __stdcall RenderPages();
		static void __stdcall RenderSlowMotion();
		static void OnMenuClosed();

	private:
		class Page;
		MenuFrameworkConfig() = default;
		static MenuFrameworkConfig &Get();
		void Render(void (Page::*a_draw)());
		void SaveNow();
		void SetStatus(std::string a_text);
		std::string Status() const;
		std::atomic<bool> _unsavedChanges{false};
		mutable std::mutex _statusLock;
		std::string _status;
	};
}
