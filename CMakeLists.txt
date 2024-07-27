cmake_minimum_required(VERSION 3.29)

project(Plugin)

add_subdirectory(clap)

clap_setup(
    VERSION
    "main"
    HELPERS_VERSION
    "main"
    )

clap_add_plugin(
    NAME
    "Gain"
    VENDOR
    "Vendor"
    ID
    "com.vendor.gain"
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
    "src/gain.cxx"
    )
