include(FetchContent)

set(ENABLE_SKYRIM_SE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_VR OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SKSE_SUPPORT_XBYAK OFF CACHE BOOL "" FORCE)
set(SKSE_SUPPORT_PATCH_SAFETY OFF CACHE BOOL "" FORCE)
set(COMMONLIB_ENABLE_IPO OFF CACHE BOOL "" FORCE)
set(COMMONLIB_PREBUILT OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	commonlibsse
	URL      https://github.com/alandtse/CommonLibVR/archive/f49373ed922fde1dfbff0208f63a49bf8846c109.tar.gz  # v16.1.0
	URL_HASH SHA256=d5f3068944afceae56c50ee0a08bf5b798cc0f13451af345046f342a660f5095
	SYSTEM
)

FetchContent_Declare(
	skse_menu_framework_api
	URL      https://github.com/QTR-Modding/SKSE-Menu-Framework-3-API/archive/1dcb70179076aae4ab626f43c5baab2735ca5877.tar.gz
	URL_HASH SHA256=cc6715ddd61b0bf0ea1578687b0f64841a868ce5a1011b4c6468e6b38ddb96eb
	SOURCE_SUBDIR no-cmake
)

FetchContent_MakeAvailable(commonlibsse skse_menu_framework_api)

if(NOT COMMAND add_commonlibsse_plugin)
	include("${commonlibsse_SOURCE_DIR}/cmake/CommonLibSSE.cmake")
endif()

find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(NanoSVG CONFIG REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS "SimpleIni.h" REQUIRED)
