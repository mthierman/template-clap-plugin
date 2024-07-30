include(FetchContent)

FetchContent_Declare(
    cmake-modules
    GIT_REPOSITORY "https://github.com/mthierman/cmake-modules.git"
    GIT_TAG main
    GIT_SHALLOW ON
    )

FetchContent_MakeAvailable(cmake-modules)

include("${cmake-modules_SOURCE_DIR}/cmake-modules.cmake")

fetch_hand(
    VERSION
    "main"
    )
