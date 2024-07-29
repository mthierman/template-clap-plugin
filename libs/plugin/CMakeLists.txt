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

add_library(
    plugin
    INTERFACE
    )

add_library(
    plugin::plugin
    ALIAS
    plugin
    )

target_compile_features(
    plugin
    INTERFACE c_std_17
              cxx_std_23
    )

target_sources(
    plugin
    INTERFACE "include/plugin/plugin.hxx"
              "include/plugin/win32.hxx"
    INTERFACE FILE_SET
              HEADERS
              BASE_DIRS
              "include"
              "${clap_SOURCE_DIR}/include"
              "${clap-helpers_SOURCE_DIR}/include"
    )

if(CMAKE_SYSTEM_NAME
   STREQUAL
   "Windows"
    )
    target_link_libraries(plugin INTERFACE glow::glow)

    target_compile_options(
        plugin
        INTERFACE $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
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
        INTERFACE
        $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
        /WX
        >
        $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:
        -Wl,/WX
        >
        )

    target_compile_definitions(
        plugin
        INTERFACE NOMINMAX
                  WIN32_LEAN_AND_MEAN
                  PLATFORM_WINDOWS=1
                  OPERATING_SYSTEM="Windows"
                  $<$<CONFIG:Debug>:HOT_RELOAD=1>
                  DEV_URL="https://localhost:5173"
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
    set(listArgs
        SOURCES
        FEATURES
        )
    cmake_parse_arguments(
        PLUGIN
        ""
        "${args}"
        "${listArgs}"
        ${ARGN}
        )

    list(
        TRANSFORM
        PLUGIN_FEATURES
        REPLACE "(.+)"
                "\"\\1\""
                OUTPUT_VARIABLE
                PLUGIN_FEATURES
        )
    list(
        JOIN
        PLUGIN_FEATURES
        ", "
        PLUGIN_FEATURES
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

    configure_file(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/config/config.hxx
        ${CMAKE_CURRENT_BINARY_DIR}/include/config/config.hxx
        )

    target_include_directories(plugin INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)

    target_compile_definitions(
        plugin
        INTERFACE PLUGIN_ID="${PLUGIN_ID}"
                  PLUGIN_NAME="${PLUGIN_NAME}"
                  PLUGIN_VENDOR="${PLUGIN_VENDOR}"
                  PLUGIN_URL="${PLUGIN_URL}"
                  PLUGIN_MANUAL_URL="${PLUGIN_MANUAL_URL}"
                  PLUGIN_SUPPORT_URL="${PLUGIN_SUPPORT_URL}"
                  PLUGIN_VERSION="${PLUGIN_VERSION}"
                  PLUGIN_DESCRIPTION="${PLUGIN_DESCRIPTION}"
                  PLUGIN_FEATURES="${PLUGIN_FEATURE_LIST}"
        )

    add_library(
        ${PLUGIN_NAME}
        MODULE
        )

    list(
        JOIN
        PLUGIN_FEATURES
        ","
        PLUGIN_FEATURE_LIST
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
