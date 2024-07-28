#include "../include/plugin/plugin.hxx"

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

        message(WM_SIZE, [hwnd = m_hwnd.get()](glow::messages::wm_size message) {
            glow::system::dbg("{} x {}", message.size().cx, message.size().cy);
            // webView.put_bounds(message.size());
            // ::SetWindowPos(hwnd, nullptr, 0, 0, message.size().cx, message.size().cy, 0);

            return 0;
        });
    }
};

glow::webview::WebViewEnvironment webViewEnvironment;
glow::webview::WebView webView;
PluginWindow pluginWindow;
} // namespace plugin::gui

namespace plugin {
//-----------------//
// clap_plugin_gui //
//-----------------//
auto Helper::guiIsApiSupported(const char* api, bool isFloating) noexcept -> bool {
    // if (isFloating) {
    //     return false;
    // }

    // if (WIN32) {
    //     if (std::strcmp(api, CLAP_WINDOW_API_WIN32) == 0) {
    //         return true;
    //     }
    // }

    return false;
}
auto Helper::guiGetPreferredApi(const char** api, bool* is_floating) noexcept -> bool {
    return false;
}

auto Helper::guiCreate(const char* api, bool isFloating) noexcept -> bool {
    plugin::gui::pluginWindow.create("Plugin", false);
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

// auto Helper::guiDestroy() noexcept -> void { }

// auto Helper::guiShow() noexcept -> bool {
//     // glow::window::show(pluginWindow.m_hwnd.get());
//     return true;
// }

// auto Helper::guiHide() noexcept -> bool {
//     // glow::window::hide(pluginWindow.m_hwnd.get());
//     return true;
// }

// auto Helper::guiSetScale(double scale) noexcept -> bool {
//     // std::cout << scale << std::endl;
//     // pluginWindow.m_scale = scale;

//     return true;
// }

// auto Helper::guiCanResize() const noexcept -> bool { return true; }

// auto Helper::guiSetSize(uint32_t width, uint32_t height) noexcept -> bool {
//     // glow::window::set_position(pluginWindow.m_hwnd.get(), 0, 0, width, height);

//     return true;
// }

// auto Helper::guiGetSize(uint32_t* width, uint32_t* height) noexcept -> bool {
//     // std::cout << "getSize: " << width << " x " << height << std::endl;
//     // *width = 200;
//     // *height = 200;

//     // return true;

//     return false;
// }

// auto Helper::guiSetParent(const clap_window* window) noexcept -> bool {
//     if (PLATFORM_WINDOWS) {
//         // ::SetWindowLongPtrA(pluginWindow.m_hwnd.get(), GWL_STYLE, WS_POPUP);
//         // glow::window::set_parent(pluginWindow.m_hwnd.get(), (::HWND)window->win32);

//         return true;
//     }

//     return false;
// }

// auto Helper::guiAdjustSize(uint32_t* width, uint32_t* height) noexcept -> bool {
//     return guiGetSize(width, height);
// }
} // namespace plugin
