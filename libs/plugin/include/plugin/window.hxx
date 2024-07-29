#pragma once

#if PLATFORM_WINDOWS
#include <glow/filesystem.hxx>
#include <glow/system.hxx>
#include <glow/webview.hxx>
#include <glow/window.hxx>

namespace plugin {
struct Window final : glow::window::Window {
    Window() {
        message(WM_CREATE, [this](glow::messages::wm_create message) {
            glow::system::dbg("WM_CREATE");

            webViewEnvironment.m_userDataFolder
                = glow::filesystem::known_folder(FOLDERID_LocalAppData, { "template-clap-plugin" });
            std::cout << webViewEnvironment.m_userDataFolder.string() << std::endl;

            webViewEnvironment.create([this]() {
                webView.create(webViewEnvironment, m_hwnd.get(), [this]() {
                    webView.navigate("about:blank");
                    webView.put_bounds(m_hwnd.get());
                });
            });

            return 0;
        });

        message(WM_NOTIFY, [this](glow::messages::wm message) {
            glow::system::dbg(
                "WM_NOTIFY: cx: {} cy: {}", LOWORD(message.lparam), HIWORD(message.lparam));

            ::SetWindowPos(
                m_hwnd.get(), nullptr, 0, 0, LOWORD(message.lparam), HIWORD(message.lparam), 0);
            webView.put_bounds(m_hwnd.get());

            return 0;
        });

        // message(WM_SIZE, [this](glow::messages::wm_size message) {
        //     glow::system::dbg("WM_SIZE: cx: {} cy: {}", message.size().cx, message.size().cy);

        //     ::SetWindowPos(m_hwnd.get(), nullptr, 0, 0, message.size().cx, message.size().cy, 0);
        //     webView.put_bounds(m_hwnd.get());

        //     return 0;
        // });

        create("PluginHelper", false);
    }

    glow::webview::WebViewEnvironment webViewEnvironment;
    glow::webview::WebView webView;
};
} // namespace plugin
#endif
