#include "gui.hxx"

#include <glow/window.hxx>

#include <iostream>

namespace plugin::gui {
struct PluginWindow final : glow::window::Window {
    PluginWindow() {
        message(WM_CREATE, [](glow::messages::wm_create message) {
            glow::system::dbg("WM_CREATE");
            return 0;
        });
    }
};

PluginWindow pluginWindow;

auto create() -> bool {
    pluginWindow.create();

    return true;
}

auto destroy() -> void { }

auto show() -> bool {
    glow::window::show(pluginWindow.m_hwnd.get());

    return true;
}

auto hide() -> bool {
    glow::window::hide(pluginWindow.m_hwnd.get());

    return true;
}

auto setScale(double scale) -> bool {
    std::cout << scale << std::endl;
    pluginWindow.m_scale = scale;

    return true;
}

auto setSize(uint32_t width, uint32_t height) -> bool {
    std::cout << "setSize: " << width << " x " << height << std::endl;

    glow::window::set_position(pluginWindow.m_hwnd.get(), 0, 0, width, height);

    return true;
}

auto setParent(const clap_window* window) -> bool {
    if (PLATFORM_WINDOWS) {
        glow::window::set_child(pluginWindow.m_hwnd.get());
        glow::window::set_parent(pluginWindow.m_hwnd.get(), (::HWND)window->win32);

        return true;
    }

    return false;
}
} // namespace plugin::gui
