#pragma once

#include <clap/ext/gui.h>

namespace plugin::gui {
auto create() -> bool;
auto destroy() -> void;
auto setParent(const clap_window* window) -> bool;
auto show() -> bool;
auto hide() -> bool;
} // namespace plugin::gui
