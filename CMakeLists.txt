cmake_minimum_required(VERSION 3.29)

project(Plugin VERSION 0.0.0 DESCRIPTION "Plugin description")

list(
    APPEND
    CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    )

include(clap)

add_clap(
    BRANCH
    "main"
    HELPERS_BRANCH
    "main"
    )

add_plugin(
    ID
    "com.gmail.mthierman"
    NAME
    ${PROJECT_NAME}
    VENDOR
    "mthierman"
    URL
    "https://github.com/mthierman"
    MANUAL_URL
    "https://github.com/mthierman"
    SUPPORT_URL
    "https://github.com/mthierman"
    VERSION
    ${PROJECT_VERSION}
    DESCRIPTION
    ${PROJECT_DESCRIPTION}
    SOURCES
    "src/plugin.cxx"
    )
