# Standard routine
cmake_minimum_required(VERSION 3.16)
project(auimusic
        VERSION 0.0.1 # CI_PROJECT_VERSION
    )

option(BUILD_SHARED_LIBS "Build shared libs" OFF)
option(AUIB_FORCE_PRECOMPILED "AUI.Boot: force precompiled dependencies" OFF)

set(AUI_VERSION v8.0.0-rc.2)

# Use AUI.Boot
file(
        DOWNLOAD
        https://raw.githubusercontent.com/aui-framework/aui/${AUI_VERSION}/aui.boot.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/aui.boot.cmake)
include(${CMAKE_CURRENT_BINARY_DIR}/aui.boot.cmake)

# import AUI
auib_import(aui https://github.com/aui-framework/aui
        COMPONENTS core views curl json crypt updater audio
        VERSION ${AUI_VERSION})

auib_import(dr_libs https://github.com/mackron/dr_libs
            VERSION 9cb7092
            ADD_SUBDIRECTORY
)

find_package(ZLIB REQUIRED)
auib_import(TagLib https://github.com/taglib/taglib
            VERSION v2.0.2)

if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(TagLib::tag INTERFACE TAGLIB_STATIC=1)
endif()

add_library(dr_libs INTERFACE IMPORTED)
target_include_directories(dr_libs INTERFACE ${dr_libs_SOURCE_DIR})


# Create the executable. This function automatically links all sources from the src/ folder,
# creates CMake target and places the resulting executable to bin/ folder.
aui_executable(${PROJECT_NAME})

# Link required libs
aui_link(${PROJECT_NAME} PRIVATE aui::core aui::views aui::updater aui::audio dr_libs TagLib::tag)

# Compile assets
aui_compile_assets(${PROJECT_NAME})

# Enable testing
aui_enable_tests(${PROJECT_NAME})

# Setup icon, display name, etc
aui_app(TARGET ${PROJECT_NAME}
        NAME "AUImusic"
        VENDOR "AUI Project"
        ICON "assets/img/icon.svg"
)
