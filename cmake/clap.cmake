function(clap_setup)
    set(args
        VERSION
        PATH
        HELPERS_VERSION
        HELPERS_PATH
        )
    cmake_parse_arguments(
        CLAP
        ""
        "${args}"
        ""
        ${ARGN}
        )

    include(FetchContent)

    if(NOT
       CLAP_LOCATION
       STREQUAL
       ""
        )
        set(FETCHCONTENT_SOURCE_DIR_CLAP ${CLAP_PATH})
    endif()

    if(NOT
       CLAP_HELPERS_LOCATION
       STREQUAL
       ""
        )
        set(FETCHCONTENT_SOURCE_DIR_CLAP-HELPERS ${CLAP_HELPERS_PATH})
    endif()

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
        clap
        INTERFACE
        )

    add_library(
        clap::clap
        ALIAS
        clap
        )

    target_sources(
        clap
        INTERFACE FILE_SET
                  HEADERS
                  BASE_DIRS
                  "${clap_SOURCE_DIR}/include"
        )

    target_sources(
        clap
        INTERFACE FILE_SET
                  HEADERS
                  BASE_DIRS
                  "${clap-helpers_SOURCE_DIR}/include"
        )

    target_compile_features(
        clap
        INTERFACE c_std_17
                  cxx_std_23
        )

    if(CMAKE_SYSTEM_NAME
       STREQUAL
       "Windows"
        )
        target_compile_options(
            clap
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
            clap
            INTERFACE
            $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
            /WX
            >
            $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:
            -Wl,/WX
            >
            )

        target_compile_definitions(
            clap
            INTERFACE NOMINMAX
                      WIN32_LEAN_AND_MEAN
            )
    endif()
endfunction()

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
        INCLUDE_DIRECTORIES
        LINK_DIRECTORIES
        LINK_LIBRARIES
        )
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
        ${PLUGIN_NAME}
        PRIVATE PLUGIN_ID="${PLUGIN_ID}"
                PLUGIN_NAME="${PLUGIN_NAME}"
                PLUGIN_VENDOR="${PLUGIN_VENDOR}"
                PLUGIN_URL="${PLUGIN_URL}"
                PLUGIN_MANUAL_URL="${PLUGIN_MANUAL_URL}"
                PLUGIN_SUPPORT_URL="${PLUGIN_SUPPORT_URL}"
                PLUGIN_VERSION="${PLUGIN_VERSION}"
                PLUGIN_DESCRIPTION="${PLUGIN_DESCRIPTION}"
        )

    target_sources(${PLUGIN_NAME} PRIVATE ${PLUGIN_SOURCES})

    target_include_directories(
        ${PLUGIN_NAME}
        PRIVATE "include"
                ${PLUGIN_INCLUDE_DIRECTORIES}
        )

    target_link_directories(
        ${PLUGIN_NAME}
        PRIVATE
        ${PLUGIN_LINK_DIRECTORIES}
        )

    target_link_libraries(
        ${PLUGIN_NAME}
        PRIVATE clap::clap
                ${PLUGIN_LINK_LIBRARIES}
        )

    target_include_directories(${PLUGIN_NAME} PRIVATE ${PLUGIN_INCLUDE_DIRECTORIES})

    if(CMAKE_SYSTEM_NAME
       STREQUAL
       "Windows"
        )
        target_sources(${PLUGIN_NAME} PRIVATE "include/plugin/gui/gui_windows.cxx")

        set_target_properties(
            ${PLUGIN_NAME}
            PROPERTIES SUFFIX
                       ".clap"
            )

        target_compile_definitions(
            ${PLUGIN_NAME}
            PRIVATE PLATFORM_WINDOWS=1
                    OPERATING_SYSTEM="Windows"
            )
    endif()
endfunction()
