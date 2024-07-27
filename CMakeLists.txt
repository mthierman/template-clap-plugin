cmake_minimum_required(VERSION 3.29)

project(Plugin)

include(cmake/clap.cmake)

clap_setup(
    VERSION
    "main"
    # PATH "$ENV{Repos}/free-audio/clap"
    HELPERS_VERSION
    "main"
    # HELPERS_PATH "$ENV{Repos}/free-audio/clap-helpers"
    )

clap_add_plugin(
    NAME
    "Gain"
    VERSION
    "0.0.0"
    DESCRIPTION
    "Gain plugin"
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
    SOURCES
    "src/gain.cxx"
    LINK_LIBRARIES
    clap::recommended
    )
