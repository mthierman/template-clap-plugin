#pragma once

#include <glow/filesystem.hxx>
#include <glow/system.hxx>
#include <glow/webview.hxx>
#include <glow/window.hxx>

namespace glow::messages {
enum struct notice : ::UINT {
    SET_SIZE = WM_APP,
};
}

namespace plugin {
struct Window final : glow::window::Window {
    Window() {
        message(WM_CREATE, [this](glow::messages::wm_create message) {
            webViewEnvironment.m_userDataFolder
                = glow::filesystem::known_folder(FOLDERID_LocalAppData, { "template-clap-plugin" });
            std::cout << webViewEnvironment.m_userDataFolder.string() << std::endl;

            webViewEnvironment.create([this]() {
                webView.create(webViewEnvironment, m_hwnd.get(), [this]() {
                    webView.navigate("https://www.example.com/");
                    webView.put_bounds(m_hwnd.get());
                });
            });

            return 0;
        });

        message(WM_WINDOWPOSCHANGED, [this](glow::messages::wm_windowposchanged message) {
            webView.put_bounds(m_hwnd.get());

            return 0;
        });

        create("PluginHelper", false);
    }

    glow::webview::WebViewEnvironment webViewEnvironment;
    glow::webview::WebView webView;
};
} // namespace plugin
