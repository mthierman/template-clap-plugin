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

auto setParent(const clap_window* window) -> bool {
    if (PLATFORM_WINDOWS) {
        glow::window::set_child(pluginWindow.m_hwnd.get());
        glow::window::set_parent(pluginWindow.m_hwnd.get(), (::HWND)window->win32);

        return true;
    }

    return false;
}

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
} // namespace plugin::gui
