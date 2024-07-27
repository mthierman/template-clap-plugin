cmake_minimum_required(VERSION 3.29)

project(Plugin)

add_subdirectory(clap)

clap_setup(
    BRANCH
    "main"
    HELPERS_BRANCH
    "main"
    )

clap_add_plugin(
    ID
    "com.vendor.gain"
    NAME
    "Gain"
    VENDOR
    "Vendor"
    URL
    "https://cleveraudio.org/"
    MANUAL_URL
    "https://cleveraudio.org/"
    SUPPORT_URL
    "https://cleveraudio.org/"
    VERSION
    "0.0.0"
    DESCRIPTION
    "Gain plugin"
    SOURCES
    "src/entry.cxx"
    )
