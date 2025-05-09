# Standard routine
cmake_minimum_required(VERSION 3.16)
project(auimusic
        VERSION 0.0.10 # CI_PROJECT_VERSION
    )

option(BUILD_SHARED_LIBS "Build shared libs" OFF)
option(AUIB_FORCE_PRECOMPILED "AUI.Boot: force precompiled dependencies" OFF)

set(AUI_VERSION 8824a340603789d2149019f351dc513bf2e22138)

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

auib_import(id3v2lib https://github.com/larsbs/id3v2lib
            VERSION 141ffb8
            ADD_SUBDIRECTORY)

add_library(dr_libs INTERFACE IMPORTED)
target_include_directories(dr_libs INTERFACE ${dr_libs_SOURCE_DIR})


# Create the executable. This function automatically links all sources from the src/ folder,
# creates CMake target and places the resulting executable to bin/ folder.
aui_executable(${PROJECT_NAME})

# Link required libs
aui_link(${PROJECT_NAME} PRIVATE aui::core aui::views aui::updater aui::audio dr_libs id3v2lib)

# Compile assets
aui_compile_assets(${PROJECT_NAME})

# Enable testing
aui_enable_tests(${PROJECT_NAME})

# Setup icon, display name, etc
aui_app(TARGET ${PROJECT_NAME}
        NAME "AUI App Example"
        VENDOR "AUI Project"
        ICON "assets/img/icon.svg"
)
