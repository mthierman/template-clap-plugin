#include "gui.hxx"

#include <glow/filesystem.hxx>
#include <glow/system.hxx>
#include <glow/webview.hxx>
#include <glow/window.hxx>

#include <iostream>

namespace plugin::gui {
glow::webview::WebViewEnvironment webViewEnvironment;
glow::webview::WebView webView;

struct PluginWindow final : glow::window::Window {
    PluginWindow() {
        message(WM_CREATE, [this](glow::messages::wm_create message) {
            glow::system::dbg("WM_CREATE, {}", ::GetACP());
            glow::system::message_box("{}", ::GetACP());

            return 0;
        });

        message(WM_SIZE, [hwnd = m_hwnd.get()](glow::messages::wm_size message) {
            glow::system::dbg("{} x {}", message.size().cx, message.size().cy);
            // webView.put_bounds(message.size());

            // ::SetWindowPos(hwnd, nullptr, 0, 0, message.size().cx, message.size().cy, 0);

            return 0;
        });
    }
};

PluginWindow pluginWindow;

auto create() -> bool {
    pluginWindow.create("Plugin", true);
    // glow::window::show(pluginWindow.m_hwnd.get());

    // webViewEnvironment.m_userDataFolder
    //     = glow::filesystem::known_folder(FOLDERID_LocalAppData, { "template-clap-plugin" });
    // std::cout << webViewEnvironment.m_userDataFolder.string() << std::endl;

    // webViewEnvironment.create([hwnd = pluginWindow.m_hwnd.get()]() {
    //     webView.create(webViewEnvironment, hwnd, [hwnd]() {
    //         webView.navigate("https://www.google.ca/");
    //         webView.put_bounds(hwnd);
    //     });
    // });

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
    glow::window::set_position(pluginWindow.m_hwnd.get(), 0, 0, width, height);

    return true;
}

auto setParent(const clap_window* window) -> bool {
    if (PLATFORM_WINDOWS) {
        ::SetWindowLongPtrA(pluginWindow.m_hwnd.get(), GWL_STYLE, WS_POPUP);
        glow::window::set_parent(pluginWindow.m_hwnd.get(), (::HWND)window->win32);

        return true;
    }

    return false;
}
} // namespace plugin::gui
