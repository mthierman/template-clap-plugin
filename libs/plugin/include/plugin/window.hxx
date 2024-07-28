#pragma once

#if PLATFORM_WINDOWS
#include <glow/filesystem.hxx>
#include <glow/system.hxx>
#include <glow/webview.hxx>
#include <glow/window.hxx>
#endif

namespace plugin {
#if PLATFORM_WINDOWS
struct Window final : glow::window::Window {
    Window() {
        message(WM_CREATE, [](glow::messages::wm_create message) {
            glow::system::dbg("WM_CREATE");

            return 0;
        });

        message(WM_SIZE, [this, hwnd = m_hwnd.get()](glow::messages::wm_size message) {
            glow::system::dbg("WM_SIZE: cx: {} cy: {}", message.size().cx, message.size().cy);

            // ::SetWindowPos(hwnd, nullptr, 0, 0, message.size().cx, message.size().cy, 0);
            // webView.put_bounds(hwnd);

            return 0;
        });
    }

    glow::webview::WebViewEnvironment webViewEnvironment;
    glow::webview::WebView webView;
};
#endif
} // namespace plugin
