project(Plugin)

include(FetchContent)

FetchContent_Declare(
    glow
    GIT_REPOSITORY "https://github.com/mthierman/Glow.git"
    GIT_TAG "next"
    GIT_SHALLOW ON
    )

FetchContent_MakeAvailable(glow)

FetchContent_Declare(
    clap
    GIT_REPOSITORY "https://github.com/free-audio/clap.git"
    GIT_TAG ${CLAP_VERSION}
    GIT_SHALLOW ON
    SOURCE_SUBDIR
    "NULL"
    )

FetchContent_MakeAvailable(clap)

FetchContent_Declare(
    clap-helpers
    GIT_REPOSITORY "https://github.com/free-audio/clap-helpers.git"
    GIT_TAG ${CLAP_HELPERS_VERSION}
    GIT_SHALLOW ON
    SOURCE_SUBDIR
    "NULL"
    )

FetchContent_MakeAvailable(clap-helpers)

add_library(plugin)

add_library(
    plugin::plugin
    ALIAS
    plugin
    )

target_compile_features(
    plugin
    PUBLIC c_std_17
           cxx_std_23
    )

target_sources(
    plugin
    PUBLIC FILE_SET
           HEADERS
           BASE_DIRS
           "include"
           "${clap_SOURCE_DIR}/include"
           "${clap-helpers_SOURCE_DIR}/include"
           FILES
           "include/plugin/plugin.hxx"
           "include/plugin/config.hxx"
           "include/plugin/window.hxx"
    )

if(CMAKE_SYSTEM_NAME
   STREQUAL
   "Windows"
    )
    target_link_libraries(plugin PUBLIC glow::glow)

    target_compile_options(
        plugin
        PUBLIC $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
               /W4
               /WX
               /wd4100
               /wd4127
               /utf-8
               /bigobj
               /diagnostics:caret
               /Zc:__cplusplus
               >
               $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:
               -Wall
               -Werror
               -Wextra
               -Wpedantic
               -Wno-unused-parameter
               -Wno-braced-scalar-init
               >
        )

    target_link_options(
        plugin
        PUBLIC
        $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
        /WX
        >
        $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:
        -Wl,/WX
        >
        )

    target_compile_definitions(
        plugin
        PUBLIC NOMINMAX
               WIN32_LEAN_AND_MEAN
               PLATFORM_WINDOWS=1
               OPERATING_SYSTEM="Windows"
        )
endif()

function(clap_add_plugin)
    set(args
        NAME
        VERSION
        DESCRIPTION
        VENDOR
        ID
        URL
        MANUAL_URL
        SUPPORT_URL
        )
    set(listArgs SOURCES)
    cmake_parse_arguments(
        PLUGIN
        ""
        "${args}"
        "${listArgs}"
        ${ARGN}
        )

    if(NOT
       PLUGIN_NAME
        )
        message(FATAL_ERROR "Plugin name was not set")
    endif()

    if(NOT
       PLUGIN_VENDOR
        )
        message(FATAL_ERROR "Plugin vendor was not set")
    endif()

    if(NOT
       PLUGIN_ID
        )
        message(FATAL_ERROR "Plugin ID was not set")
    endif()

    add_library(
        ${PLUGIN_NAME}
        MODULE
        )

    target_compile_definitions(
        plugin
        PUBLIC PLUGIN_ID="${PLUGIN_ID}"
               PLUGIN_NAME="${PLUGIN_NAME}"
               PLUGIN_VENDOR="${PLUGIN_VENDOR}"
               PLUGIN_URL="${PLUGIN_URL}"
               PLUGIN_MANUAL_URL="${PLUGIN_MANUAL_URL}"
               PLUGIN_SUPPORT_URL="${PLUGIN_SUPPORT_URL}"
               PLUGIN_VERSION="${PLUGIN_VERSION}"
               PLUGIN_DESCRIPTION="${PLUGIN_DESCRIPTION}"
        )

    target_sources(${PLUGIN_NAME} PRIVATE ${PLUGIN_SOURCES})

    target_link_libraries(${PLUGIN_NAME} PRIVATE plugin::plugin)

    if(CMAKE_SYSTEM_NAME
       STREQUAL
       "Windows"
        )
        set_target_properties(
            ${PLUGIN_NAME}
            PROPERTIES SUFFIX
                       ".clap"
            )
    endif()
endfunction()
