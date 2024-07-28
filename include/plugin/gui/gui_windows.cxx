#include "gui.hxx"

#include <glow/filesystem.hxx>
#include <glow/system.hxx>
#include <glow/webview.hxx>
#include <glow/window.hxx>

#include <iostream>

namespace plugin::gui {
struct PluginWindow final : glow::window::Window {
    PluginWindow() {
        message(WM_CREATE, [this](glow::messages::wm_create message) {
            glow::system::dbg("WM_CREATE");

            return 0;
        });

        message(WM_SIZE, [](glow::messages::wm_size message) {
            glow::system::dbg("{} x {}", message.size().cx, message.size().cy);

            return 0;
        });

        create();
    }

    // wil::unique_couninitialize_call coInit { glow::system::co_initialize() };
};

glow::webview::WebViewEnvironment m_webViewEnvironment;
glow::webview::WebView m_webView;
PluginWindow pluginWindow;

auto create() -> bool {
    pluginWindow.create();

    m_webViewEnvironment.m_userDataFolder
        = glow::filesystem::known_folder(FOLDERID_LocalAppData, { "ClapPlugin" });
    std::cout << m_webViewEnvironment.m_userDataFolder.string() << std::endl;

    // if (m_hwnd.get()) {
    //     std::cout << "Window handle exists!" << std::endl;
    // }

    m_webViewEnvironment.create([=]() {
        m_webView.create(m_webViewEnvironment, pluginWindow.m_hwnd.get(), []() {
            m_webView.navigate("https://www.google.ca/");
            m_webView.put_bounds(pluginWindow.m_hwnd.get());
        });
    });

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
    // ::SendMessage(pluginWindow.m_hwnd.get(), WM_SIZE, 0, 0);

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
