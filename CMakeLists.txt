cmake_minimum_required(VERSION 3.25)

set(AUTHOR_NAME "Yuuri")
project(OrbitMenu VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/MP)

set(ENABLE_SKYRIM_SE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_VR OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SKSE_SUPPORT_XBYAK OFF CACHE BOOL "" FORCE)
set(SKSE_SUPPORT_PATCH_SAFETY OFF CACHE BOOL "" FORCE)
set(COMMONLIB_ENABLE_IPO OFF CACHE BOOL "" FORCE)
set(COMMONLIB_PREBUILT OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
	commonlibsse
	URL https://github.com/alandtse/CommonLibVR/archive/f49373ed922fde1dfbff0208f63a49bf8846c109.tar.gz
	URL_HASH SHA256=d5f3068944afceae56c50ee0a08bf5b798cc0f13451af345046f342a660f5095
	SYSTEM
)

FetchContent_Declare(
	skse_menu_framework_api
	URL https://github.com/QTR-Modding/SKSE-Menu-Framework-3-API/archive/1dcb70179076aae4ab626f43c5baab2735ca5877.tar.gz
	URL_HASH SHA256=cc6715ddd61b0bf0ea1578687b0f64841a868ce5a1011b4c6468e6b38ddb96eb
	SOURCE_SUBDIR no-cmake
)

FetchContent_MakeAvailable(commonlibsse skse_menu_framework_api)
include(${commonlibsse_SOURCE_DIR}/cmake/CommonLibSSE.cmake)

find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(NanoSVG CONFIG REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS SimpleIni.h REQUIRED)

configure_file(cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/generated/version.rc @ONLY)

set(font ${CMAKE_CURRENT_SOURCE_DIR}/fonts/Jost-Medium.ttf)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${font})
file(READ ${font} font_bytes HEX)
string(REPEAT "[0-9a-f]" 64 font_line)
string(REGEX REPLACE "(${font_line})" "\\1\n" font_bytes "${font_bytes}")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," font_bytes "${font_bytes}")
file(CONFIGURE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/JostMedium.h
	CONTENT "#pragma once\n\ninline constexpr unsigned char kJostMedium[]{\n@font_bytes@};\n" @ONLY)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX src FILES ${sources})

add_commonlibsse_plugin(${PROJECT_NAME} AUTHOR ${AUTHOR_NAME} SOURCES ${sources} ${CMAKE_CURRENT_BINARY_DIR}/generated/version.rc)

target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)
target_include_directories(${PROJECT_NAME} PRIVATE src ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${skse_menu_framework_api_SOURCE_DIR} ${SIMPLEINI_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui nlohmann_json::nlohmann_json NanoSVG::nanosvg NanoSVG::nanosvgrast)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	UNICODE
	_UNICODE
	NOMINMAX
	WIN32_LEAN_AND_MEAN
	IMGUI_DEFINE_MATH_OPERATORS
	_CRT_SECURE_NO_WARNINGS
	_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive- /utf-8 /Zc:inline /Zc:__cplusplus /external:anglebrackets /external:W0 /Zi)
target_link_options(${PROJECT_NAME} PRIVATE /DEBUG:FULL /OPT:REF /OPT:ICF /INCREMENTAL:NO)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION SKSE/Plugins COMPONENT OrbitMenu)
install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION SKSE/Plugins COMPONENT OrbitMenu OPTIONAL)
install(FILES dist/SKSE/Plugins/OrbitMenu_Default.ini DESTINATION "SKSE/Plugins/Orbit Menu" COMPONENT OrbitMenu)
install(FILES fonts/OFL.txt DESTINATION "SKSE/Plugins/Orbit Menu/fonts" COMPONENT OrbitMenu)
