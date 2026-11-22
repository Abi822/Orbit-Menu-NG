# Orbit Menu

An 8-slot Quick Access Menu inspired by The Witcher 3

## Requirements

| Mod | Notes |
|---|---|
| [SKSE64](https://skse.silverlock.org/) | Plugin Loader |
| [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444) |
| [SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604) | For assigning ro slots in inventory and Magic menu |
| [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352) | **optional** |
| [Inventory Interface Information Injector](https://www.nexusmods.com/skyrimspecialedition/mods/85702 (I4)) | **optional** |

## Building

Needs Visual Studio 2022 (or its Build Tools) with **Desktop development with C++**, and
[vcpkg](https://github.com/microsoft/vcpkg) with `VCPKG_ROOT` set.

```powershell
cmake --preset vs2022
cmake --build --preset release
cmake --install build/vs2022 --config Release --component OrbitMenu --prefix "build/Orbit Menu"
```
