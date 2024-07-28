#pragma once

#include <clap/ext/gui.h>

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace plugin::gui {
auto def_window_proc(::HWND hwnd, ::UINT msg, ::WPARAM wparam, ::LPARAM lparam) -> ::LRESULT {
    return ::DefWindowProcA(hwnd, msg, wparam, lparam);
}

auto CALLBACK window_proc(::HWND hwnd, ::UINT msg, ::WPARAM wparam, ::LPARAM lparam) -> ::LRESULT {
    switch (msg) {
        case WM_NCCREATE: {
            auto create { reinterpret_cast<::CREATESTRUCTA*>(lparam) };

            if (auto self { static_cast<Window*>(create->lpCreateParams) }; self) {
                ::SetWindowLongPtrA(hwnd, 0, reinterpret_cast<::LONG_PTR>(self));
                self->m_hwnd.reset(hwnd);
                glow::window::get_placement(hwnd, &self->m_windowPlacement);
            }
        } break;
        case WM_NCDESTROY: {
            if (auto self { reinterpret_cast<Window*>(::GetWindowLongPtrA(hwnd, 0)) }; self) {
                ::SetWindowLongPtrA(hwnd, 0, reinterpret_cast<::LONG_PTR>(nullptr));
            }
        } break;
    }

    if (auto self { reinterpret_cast<Window*>(::GetWindowLongPtrA(hwnd, 0)) }; self) {
        if (self->message(msg)) {
            return self->message({ hwnd, msg, wparam, lparam });
        } else {
            switch (msg) {
                case WM_CLOSE: {
                    self->m_hwnd.reset();
                    return 0;
                } break;
            }
        }
    }

    return def_window_proc(hwnd, msg, wparam, lparam);
}

auto create() -> bool {
    ::WNDCLASSEXA m_windowClass {
        .cbSize { sizeof(::WNDCLASSEXA) },
        .style { 0 },
        .lpfnWndProc { window_proc },
        .cbClsExtra { 0 },
        .cbWndExtra { sizeof(Window) },
        .hInstance { glow::system::get_instance() },
        .hIcon { glow::system::load_resource_icon() ? glow::system::load_resource_icon()
                                                    : glow::system::load_system_icon() },
        .hCursor { glow::system::load_system_cursor() },
        .hbrBackground { glow::system::load_system_brush() },
        .lpszMenuName { nullptr },
        .lpszClassName { "Window" },
        .hIconSm { glow::system::load_resource_icon() ? glow::system::load_resource_icon()
                                                      : glow::system::load_system_icon() }
    };

    if (::GetClassInfoExA(glow::system::get_instance(), windowClass->lpszClassName, windowClass)
        == 0) {
        ::RegisterClassExA(windowClass);
    }

    return true;
}

auto destroy() -> void { }

auto setParent(const clap_window* window) -> bool {
    if (PLATFORM_WINDOWS) {
        return true;
    }

    return false;
}
} // namespace plugin::gui
