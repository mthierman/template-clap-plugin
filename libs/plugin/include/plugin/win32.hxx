#pragma once

#include <glow/filesystem.hxx>
#include <glow/system.hxx>
#include <glow/webview.hxx>
#include <glow/window.hxx>

// namespace plugin {
// struct Window final : glow::window::Window {
//     Window() {
//         message(WM_CREATE, [this](glow::messages::wm_create message) {
//             webViewEnvironment.m_userDataFolder
//                 = glow::filesystem::known_folder(FOLDERID_LocalAppData, { "template-clap-plugin"
//                 });
//             std::cout << webViewEnvironment.m_userDataFolder.string() << std::endl;

//             webViewEnvironment.create([this]() {
//                 webView.create(webViewEnvironment, m_hwnd.get(), [this]() {
//                     webView.navigate("https://www.example.com/");
//                     webView.put_bounds(m_hwnd.get());
//                 });
//             });

//             return 0;
//         });

//         message(WM_WINDOWPOSCHANGED, [this](glow::messages::wm_windowposchanged message) {
//             webView.put_bounds(m_hwnd.get());

//             return 0;
//         });

//         create();
//     }

//     glow::webview::WebViewEnvironment webViewEnvironment;
//     glow::webview::WebView webView;
// };
// } // namespace plugin

namespace plugin {
struct Window {
    Window() {
        ::HMODULE module;
        ::GetModuleHandleExW(0, nullptr, &module);

        ::WNDCLASSEXW windowClass {
            .cbSize { sizeof(::WNDCLASSEXW) },
            .style { 0 },
            .lpfnWndProc { window_proc },
            .cbClsExtra { 0 },
            .cbWndExtra { sizeof(Window) },
            .hInstance { module },
            .hIcon { nullptr },
            .hCursor { static_cast<::HCURSOR>(
                ::LoadImageA(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED | LR_DEFAULTSIZE)) },
            .hbrBackground { static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH)) },
            .lpszMenuName { nullptr },
            .lpszClassName { L"Window" },
            .hIconSm { nullptr }
        };

        ::RegisterClassExW(&windowClass);

        auto hwnd { ::CreateWindowExW(0,
                                      windowClass.lpszClassName,
                                      windowClass.lpszClassName,
                                      WS_POPUP,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      nullptr,
                                      nullptr,
                                      glow::system::get_instance(),
                                      this) };

        ::ShowWindow(hwnd, SW_SHOW);
    }

    static auto CALLBACK window_proc(::HWND hwnd,
                                     ::UINT msg,
                                     ::WPARAM wparam,
                                     ::LPARAM lparam) -> ::LRESULT {
        switch (msg) {
            case WM_NCCREATE: {
                auto create { reinterpret_cast<::CREATESTRUCTW*>(lparam) };

                if (auto self { static_cast<Window*>(create->lpCreateParams) }; self) {
                    ::SetWindowLongPtrW(hwnd, 0, reinterpret_cast<::LONG_PTR>(self));
                    self->m_hwnd.reset(hwnd);
                }
            } break;
            case WM_NCDESTROY: {
                if (auto self { reinterpret_cast<Window*>(::GetWindowLongPtrW(hwnd, 0)) }; self) {
                    ::SetWindowLongPtrW(hwnd, 0, reinterpret_cast<::LONG_PTR>(nullptr));
                }
            } break;
        }

        if (auto self { reinterpret_cast<Window*>(::GetWindowLongPtrA(hwnd, 0)) }; self) {
            switch (msg) {
                case WM_CLOSE: {
                    self->m_hwnd.reset();
                    return 0;
                } break;
            }
        }

        return ::DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    wil::unique_hwnd m_hwnd;
};
} // namespace plugin
