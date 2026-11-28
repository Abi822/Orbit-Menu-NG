#pragma once

#include "Config/Settings.h"
#include <SimpleIni.h>

namespace Orbit::Config
{
	class OrbitMenuIni
	{
	public:
		static inline const std::filesystem::path kDefaultsPath{"Data/SKSE/Plugins/Orbit Menu/OrbitMenu_Default.ini"};
		static inline const std::filesystem::path kUserPath{"Data/SKSE/Plugins/Orbit Menu/OrbitMenu.ini"};

		OrbitMenuIni() { _ini.SetUnicode(); }

		[[nodiscard]] SI_Error Load(const std::filesystem::path &a_path) { return _ini.LoadFile(a_path.c_str()); }
		void CopyDefaultsFile();
		void RemoveOldKeys();
		void Read(Settings &a_settings) const;
		void Write(Settings a_settings);
		[[nodiscard]] bool SaveAs(const std::filesystem::path &a_path) const;

	private:
		CSimpleIniA _ini;
	};
}
