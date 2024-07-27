cmake_minimum_required(VERSION 3.29)

project(Plugin VERSION 0.0.0 DESCRIPTION "Plugin description")

add_subdirectory(clap)

clap_setup(
    BRANCH
    "main"
    HELPERS_BRANCH
    "main"
    )

clap_add_plugin(
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
