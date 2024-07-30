#pragma once

#include <glow/filesystem.hxx>
#include <glow/system.hxx>
#include <glow/webview.hxx>
#include <glow/window.hxx>

namespace plugin {
struct Window final : glow::window::Window {
    Window() {
        // auto ctx = ::GetThreadDpiAwarenessContext();
        // auto aware = ::GetAwarenessFromDpiAwarenessContext(ctx);

        // if (ctx == DPI_AWARENESS_CONTEXT_UNAWARE) {
        //     glow::system::message_box("DPI_AWARENESS_CONTEXT_UNAWARE");
        // }
        // if (ctx == DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) {
        //     glow::system::message_box("DPI_AWARENESS_CONTEXT_SYSTEM_AWARE");
        // }
        // if (ctx == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE) {
        //     glow::system::message_box("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE");
        // }
        // if (ctx == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) {
        //     glow::system::message_box("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2");
        // }
        // if (ctx == DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED) {
        //     glow::system::message_box("DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED");
        // }

        // switch (aware) {
        //     case DPI_AWARENESS_INVALID: {
        //         glow::system::message_box("DPI_AWARENESS_INVALID");
        //     } break;
        //     case DPI_AWARENESS_UNAWARE: {
        //         glow::system::message_box("DPI_AWARENESS_UNAWARE");
        //     } break;
        //     case DPI_AWARENESS_SYSTEM_AWARE: {
        //         glow::system::message_box("DPI_AWARENESS_SYSTEM_AWARE");
        //     } break;
        //     case DPI_AWARENESS_PER_MONITOR_AWARE: {
        //         glow::system::message_box("DPI_AWARENESS_PER_MONITOR_AWARE");
        //     } break;
        // }

        webViewEnvironment.m_userDataFolder
            = glow::filesystem::known_folder(FOLDERID_LocalAppData, { "template-clap-plugin" });

        message(WM_CREATE, [this](glow::messages::wm_create message) {
            glow::window::set_position(m_hwnd.get(), 0, 0, 640, 480);

            webViewEnvironment.create([this]() {
                webView.create(webViewEnvironment, m_hwnd.get(), [this]() {
#if HOT_RELOAD
                    webView.navigate(DEV_URL);
#else
                    webView.navigate("https://www.example.com/");
#endif
                    webView.put_bounds(m_hwnd.get());
                });
            });

            return 0;
        });

        message(WM_WINDOWPOSCHANGED, [this](glow::messages::wm_windowposchanged message) {
            webView.put_bounds(m_hwnd.get());

            return 0;
        });

        message(WM_DESTROY, [this](glow::messages::wm message) {
            webView.close();
            webViewEnvironment.close();

            return 0;
        });
    }

    glow::webview::WebViewEnvironment webViewEnvironment;
    glow::webview::WebView webView;
};
} // namespace plugin
