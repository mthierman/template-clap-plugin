#pragma once

#include <clap/ext/gui.h>

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace plugin::gui {
auto create() -> bool { return true; }

auto destroy() -> void { }

auto setParent(const clap_window* window) -> bool {
    if (PLATFORM_WINDOWS) {
        return true;
    }

    return false;
}
} // namespace plugin::gui
