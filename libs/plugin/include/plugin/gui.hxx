#pragma once

#include <clap/ext/gui.h>

namespace plugin::gui {
auto create() -> bool;
auto destroy() -> void;
auto show() -> bool;
auto hide() -> bool;
auto setScale(double scale) -> bool;
auto setSize(uint32_t width, uint32_t height) -> bool;
auto setParent(const clap_window* window) -> bool;
} // namespace plugin::gui
