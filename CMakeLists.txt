cmake_minimum_required(VERSION 3.29)

project(TemplateClapPlugin)

# include(FetchContent)

# FetchContent_Declare(
#     modules
#     GIT_REPOSITORY "https://github.com/mthierman/cmake-modules.git"
#     GIT_TAG main
#     GIT_SHALLOW ON
#     )

# FetchContent_MakeAvailable(modules)

# include(${modules_SOURCE_DIR}/cmake-modules.cmake)

# install_glow(0.0.8)

add_subdirectory(libs/plugin)
add_subdirectory(plugins/gain)
