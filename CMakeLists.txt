cmake_minimum_required(VERSION 3.29)

project(Plugin)

list(
    APPEND
    CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/libs/cmake-modules/"
    )

include(cmake-modules)
include(FetchContent)

FetchContent_Declare(clap URL https://github.com/free-audio/clap/archive/refs/heads/main.zip)
FetchContent_MakeAvailable(clap)

FetchContent_Declare(
    clap-helpers URL https://github.com/free-audio/clap-helpers/archive/refs/heads/main.zip
    )
FetchContent_MakeAvailable(clap-helpers)

install_common()

add_library(
    ${PROJECT_NAME}
    MODULE
    )

target_sources(${PROJECT_NAME} PRIVATE "src/entry.cxx")

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE common::flags
            common::definitions
            common::features
            clap
            clap-helpers
    )

if(CMAKE_SYSTEM_NAME
   STREQUAL
   "Windows"
    )
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES SUFFIX
                   ".clap"
        )
endif()
