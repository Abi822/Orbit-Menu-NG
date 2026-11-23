cmake_minimum_required(VERSION 3.25)

cmake_policy(SET CMP0135 NEW)

project(
	OrbitMenu
	VERSION 0.1.0
	DESCRIPTION "Orbit Menu, a Witcher 3 style radial menu for Skyrim SE/AE"
	LANGUAGES CXX
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(FATAL_ERROR "In-source builds are not allowed. Configure with: cmake --preset vs2022")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC AND CMAKE_GENERATOR MATCHES "Visual Studio")
	add_compile_options(/MP)
endif()

include(cmake/Dependencies.cmake)

configure_file(cmake/Version.h.in "${CMAKE_CURRENT_BINARY_DIR}/generated/Version.h" @ONLY)
configure_file(cmake/version.rc.in "${CMAKE_CURRENT_BINARY_DIR}/generated/version.rc" @ONLY)

set(JOST_FONT "${CMAKE_CURRENT_SOURCE_DIR}/fonts/Jost-Medium.ttf")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${JOST_FONT}")
file(READ "${JOST_FONT}" JOST_BYTES HEX)
string(REPEAT "[0-9a-f]" 64 JOST_LINE)
string(REGEX REPLACE "(${JOST_LINE})" "\\1\n" JOST_BYTES "${JOST_BYTES}")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," JOST_BYTES "${JOST_BYTES}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated/JostMedium.h"
	CONTENT "#pragma once\n\ninline constexpr unsigned char kJostMedium[]{\n@JOST_BYTES@};\n" @ONLY)

set(SOURCES
	src/PCH.h
	src/main.cpp

	src/Core/Slots.h
	src/Core/OrbitMath.h
	src/Core/OrbitState.h
	src/Core/OrbitState.cpp
	src/Core/SlotView.h

	src/Config/Settings.h
	src/Config/Settings.cpp
	src/Config/Translation.h
	src/Config/Translation.cpp
	src/Config/MenuFrameworkConfig.h
	src/Config/MenuFrameworkConfig.cpp

	src/Input/KeyBinds.h
	src/Input/KeyBinds.cpp
	src/Input/InputManager.h
	src/Input/InputManager.cpp

	src/Game/GameState.h
	src/Game/GameState.cpp
	src/Game/Hooks.h
	src/Game/Hooks.cpp
	src/Game/SlowMotion.h
	src/Game/SlowMotion.cpp

	src/UI/ImGuiLayer.h
	src/UI/ImGuiLayer.cpp
	src/UI/Graphics/Font.h
	src/UI/Graphics/Font.cpp
	src/UI/Graphics/GpuTexture.h
	src/UI/Graphics/GpuTexture.cpp
	src/UI/Graphics/SwfIcon.h
	src/UI/Graphics/SwfIcon.cpp
	src/UI/Graphics/TextMeasure.h
	src/UI/Graphics/TextMeasure.cpp
	src/UI/OrbitMenu/OrbitRenderer.h
	src/UI/OrbitMenu/OrbitRenderer.cpp
	src/UI/OrbitMenu/MenuAnimation.h
	src/UI/OrbitMenu/MenuAnimation.cpp
	src/UI/OrbitMenu/MenuLayout.h
	src/UI/OrbitMenu/MenuPalette.h
	src/UI/OrbitMenu/MenuPalette.cpp
	src/UI/OrbitMenu/MenuContent.h
	src/UI/OrbitMenu/MenuContent.cpp
	src/UI/OrbitMenu/ItemDescriptionLayout.h
	src/UI/OrbitMenu/ItemDescriptionLayout.cpp
	src/UI/OrbitMenu/IconCache.h
	src/UI/OrbitMenu/IconCache.cpp
	src/UI/OrbitMenu/MenuPainter.h
	src/UI/OrbitMenu/MenuPainter.cpp
	src/UI/OrbitMenu/OrbitMenuCurved.h
	src/UI/Skins/Skin.h
	src/UI/Skins/Skin.cpp
	src/UI/Skins/SkinIconNames.h
	src/UI/Skins/SkinIconNames.cpp
	src/UI/Skins/SvgImage.h
	src/UI/Skins/SvgImage.cpp
	src/UI/Skins/SkinImages.h
	src/UI/Skins/SkinImages.cpp

	src/Items/ItemKind.h
	src/Items/SlotStore.h
	src/Items/SlotStore.cpp
	src/Items/Pages.h
	src/Items/Pages.cpp
	src/Items/Inventory.h
	src/Items/Inventory.cpp
	src/Items/Assignment.h
	src/Items/Assignment.cpp
	src/Items/SlotViewBuilder.h
	src/Items/SlotViewBuilder.cpp
	src/Items/EquipController.h
	src/Items/EquipController.cpp
	src/Items/ItemIcons.h
	src/Items/ItemIcons.cpp
	src/Items/ItemDescription.h
	src/Items/ItemDescription.cpp

	src/Integration/MenuFrameworkBridge.h
	src/Integration/MenuFrameworkBridge.cpp

	src/Integration/I4Addon.h
	src/Integration/I4Addon.cpp
)

add_commonlibsse_plugin(
	${PROJECT_NAME}
	NAME "${PROJECT_NAME}"
	USE_ADDRESS_LIBRARY
	SOURCES ${SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/generated/version.rc"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${SOURCES})

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/src"
		"${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_include_directories(
	${PROJECT_NAME}
	SYSTEM PRIVATE
		"${skse_menu_framework_api_SOURCE_DIR}"
		"${SIMPLEINI_INCLUDE_DIRS}"
)

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		UNICODE
		_UNICODE
		NOMINMAX
		WIN32_LEAN_AND_MEAN
		IMGUI_DEFINE_MATH_OPERATORS
		_CRT_SECURE_NO_WARNINGS
		_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING  # SKSEMenuFramework.h uses <codecvt>
)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui nlohmann_json::nlohmann_json NanoSVG::nanosvg NanoSVG::nanosvgrast)

target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

if(MSVC)
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			/W4
			/permissive-
			/utf-8
			/Zc:inline
			/Zc:__cplusplus
			/external:anglebrackets
			/external:W0
			/Zi  # PDB for crash logs
	)

	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			/DEBUG:FULL
			"$<$<CONFIG:Release>:/OPT:REF;/OPT:ICF;/INCREMENTAL:NO>"
	)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "SKSE/Plugins" COMPONENT OrbitMenu)
install(FILES "$<TARGET_PDB_FILE:${PROJECT_NAME}>" DESTINATION "SKSE/Plugins" COMPONENT OrbitMenu OPTIONAL)
install(FILES "dist/SKSE/Plugins/OrbitMenu_Default.ini" DESTINATION "SKSE/Plugins" COMPONENT OrbitMenu)
install(FILES "fonts/Jost-OFL.txt" DESTINATION "SKSE/Plugins/Orbit Menu/fonts" COMPONENT OrbitMenu)
