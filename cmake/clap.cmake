include(FetchContent)

FetchContent_Declare(
    clap
    GIT_REPOSITORY "https://github.com/free-audio/clap.git"
    GIT_TAG "main"
    GIT_SHALLOW ON
    SOURCE_SUBDIR
    "asd"
    )

FetchContent_MakeAvailable(clap)

FetchContent_Declare(
    clap-helpers
    GIT_REPOSITORY "https://github.com/free-audio/clap-helpers.git"
    GIT_TAG "main"
    GIT_SHALLOW ON
    SOURCE_SUBDIR
    "asd"
    )

FetchContent_MakeAvailable(clap-helpers)

add_library(
    thunderclap
    INTERFACE
    )

add_library(
    mthierman::thunderclap
    ALIAS
    thunderclap
    )

target_sources(
    thunderclap
    INTERFACE FILE_SET
              HEADERS
              BASE_DIRS
              "${clap_SOURCE_DIR}/include"
    )

target_sources(
    thunderclap
    INTERFACE FILE_SET
              HEADERS
              BASE_DIRS
              "${clap-helpers_SOURCE_DIR}/include"
    )

function(add_plugin)
    set(args
        ID
        NAME
        VENDOR
        URL
        MANUAL_URL
        SUPPORT_URL
        VERSION
        DESCRIPTION
        )
    set(listArgs SOURCES)
    cmake_parse_arguments(
        PLUGIN
        ""
        "${args}"
        "${listArgs}"
        ${ARGN}
        )

    message(STATUS ${PLUGIN_ID})
    message(STATUS ${PLUGIN_NAME})
    message(STATUS ${PLUGIN_VENDOR})
    message(STATUS ${PLUGIN_URL})
    message(STATUS ${PLUGIN_MANUAL_URL})
    message(STATUS ${PLUGIN_SUPPORT_URL})
    message(STATUS ${PLUGIN_VERSION})
    message(STATUS ${PLUGIN_DESCRIPTION})

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

    target_compile_features(
        ${PLUGIN_NAME}
        PRIVATE c_std_17
                cxx_std_23
        )

    target_link_libraries(${PROJECT_NAME} PRIVATE mthierman::thunderclap)

    if(CMAKE_SYSTEM_NAME
       STREQUAL
       "Windows"
        )
        set_target_properties(
            ${PLUGIN_NAME}
            PROPERTIES SUFFIX
                       ".clap"
            )

        target_compile_options(
            ${PROJECT_NAME}
            PRIVATE $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
                    /W4
                    /WX
                    /wd4100
                    /wd4101
                    /wd4127
                    /wd4189
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
                    >
            )

        target_link_options(
            ${PROJECT_NAME}
            PRIVATE
            $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
            /WX
            >
            $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:
            -Wl,/WX
            >
            )

        target_compile_definitions(
            ${PROJECT_NAME}
            PRIVATE NOMINMAX
                    WIN32_LEAN_AND_MEAN
                    GDIPVER=0x0110
            )
    endif()
endfunction()
