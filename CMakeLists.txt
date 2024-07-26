cmake_minimum_required(VERSION 3.29)

project(Plugin)

list(
    APPEND
    CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/libs/cmake-modules/"
    )

include(FetchContent)

FetchContent_Declare(
    clap
    GIT_REPOSITORY "https://github.com/free-audio/clap.git"
    GIT_TAG "main"
    GIT_SHALLOW ON
    )

FetchContent_MakeAvailable(clap)

FetchContent_Declare(
    clap-helpers
    GIT_REPOSITORY "https://github.com/free-audio/clap-helpers.git"
    GIT_TAG "main"
    GIT_SHALLOW ON
    )

FetchContent_MakeAvailable(clap-helpers)

include(clap_plugin)

add_plugin(
    NAME
    ${PROJECT_NAME}
    SOURCES
    "src/entry.cxx"
    )

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE clap
            clap-helpers
    )

target_compile_features(
    ${PROJECT_NAME}
    PRIVATE c_std_17
            cxx_std_23
    )

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:
            /W4
            /WX
            /utf-8
            /bigobj
            /diagnostics:caret
            /Zc:__cplusplus
            >
            $<$<CXX_COMPILER_ID:Clang>:
            -Wall
            -Werror
            -Wextra
            -Wpedantic
            >
    )

target_link_options(
    ${PROJECT_NAME}
    PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
    /entry:mainCRTStartup
    /WX
    >
    $<$<CXX_COMPILER_ID:Clang>:
    -Wl,/entry:mainCRTStartup,/WX
    >
    )

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE NOMINMAX
            WIN32_LEAN_AND_MEAN
            GDIPVER=0x0110
    )
