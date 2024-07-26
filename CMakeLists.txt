cmake_minimum_required(VERSION 3.29)

project(Plugin)

list(
    APPEND
    CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/libs/cmake-modules/"
    )

include(cmake-modules)
install_common()
install_clap(main)
install_clap_helpers(main)

include(clap_plugin)

add_plugin(
    NAME
    ${PROJECT_NAME}
    SOURCES
    "src/entry.cxx"
    )
