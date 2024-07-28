#pragma once

#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <functional>
#include <unordered_map>
#include <vector>

#if PLATFORM_WINDOWS
#include <glow/filesystem.hxx>
#include <glow/system.hxx>
#include <glow/webview.hxx>
#include <glow/window.hxx>
#include <iostream>

namespace plugin::gui {
struct Window final : glow::window::Window {
    Window() {
        message(WM_CREATE, [this](glow::messages::wm_create message) {
            glow::system::dbg("WM_CREATE");

            return 0;
        });

        message(WM_SIZE, [hwnd = m_hwnd.get()](glow::messages::wm_size message) {
            glow::system::dbg("cx: {} cy: {}", message.size().cx, message.size().cy);

            return 0;
        });
    }

    glow::webview::WebViewEnvironment webViewEnvironment;
    glow::webview::WebView webView;
};
} // namespace plugin::gui
#endif

namespace plugin {
using TerminateMax = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                           clap::helpers::CheckingLevel::Maximal>;
using TerminateMin = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                           clap::helpers::CheckingLevel::Minimal>;
using TerminateNone = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                            clap::helpers::CheckingLevel::None>;
using IgnoreMax = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                        clap::helpers::CheckingLevel::Maximal>;
using IgnoreMin = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                        clap::helpers::CheckingLevel::Minimal>;
using IgnoreNone = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                         clap::helpers::CheckingLevel::None>;
using ParameterToValue = std::unordered_map<clap_id, double*>;
using Features = std::vector<const char*>;

template <typename T> struct Helper : public IgnoreNone {
    using IgnoreNone::IgnoreNone;

    //--------------------//
    // clap_plugin_params //
    //--------------------//
    clap_id nParams { 0 };
    plugin::ParameterToValue paramToValue;
    auto paramsCount() const noexcept -> uint32_t override { return nParams; }

    //-----------------//
    // clap_plugin_gui //
    //-----------------//
    auto guiIsApiSupported(const char* api, bool isFloating) noexcept -> bool override {
        if (isFloating) {
            return false;
        }

        if (PLATFORM_WINDOWS) {
            if (std::strcmp(api, CLAP_WINDOW_API_WIN32) == 0) {
                return true;
            }
        }

        return false;
    }

    auto guiGetPreferredApi(const char** api, bool* is_floating) noexcept -> bool override {
        return false;
    }

    auto guiCreate(const char* api, bool isFloating) noexcept -> bool override {
        m_window.create("plugin", true);
        glow::window::show(m_window.m_hwnd.get());

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

    auto guiDestroy() noexcept -> void override { }

    auto guiSetParent(const clap_window* window) noexcept -> bool override {
        glow::system::dbg("guiSetParent");

        if (PLATFORM_WINDOWS) {
            ::SetWindowLongPtrA(m_window.m_hwnd.get(), GWL_STYLE, WS_POPUP);
            glow::window::set_parent(m_window.m_hwnd.get(), (::HWND)window->win32);

            return true;
        }

        return false;
    }

    auto guiShow() noexcept -> bool override {
        glow::window::show(m_window.m_hwnd.get());

        return true;
    }

    auto guiHide() noexcept -> bool override {
        glow::window::hide(m_window.m_hwnd.get());

        return true;
    }

    auto guiSetScale(double scale) noexcept -> bool override {
        std::cout << scale << std::endl;
        m_window.m_scale = scale;

        return true;
    }

    auto guiCanResize() const noexcept -> bool override { return true; }

    auto guiSetSize(uint32_t width, uint32_t height) noexcept -> bool override {
        glow::system::dbg("{} x {}", width, height);

        // glow::window::set_position(pluginWindow.m_hwnd.get(), 0, 0, width, height);

        return true;
    }

    auto guiGetSize(uint32_t* width, uint32_t* height) noexcept -> bool override {
        glow::system::dbg("{} x {}", *width, *height);

        *width = 200;
        *height = 200;

        return true;
    }

    auto guiAdjustSize(uint32_t* width, uint32_t* height) noexcept -> bool override {
        return guiGetSize(width, height);
    }

    // auto guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept -> bool override;
    // auto guiSuggestTitle(const char* title) noexcept -> void override;
    // auto guiSetTransient(const clap_window* window) noexcept -> bool override;

    //-------------------------//
    // clap_plugin_audio_ports //
    //-------------------------//
    auto audioPortsCount(bool isInput) const noexcept -> uint32_t override { return 1; }
    auto audioPortsInfo(uint32_t index,
                        bool isInput,
                        clap_audio_port_info* info) const noexcept -> bool override {
        if (index > 0)
            return false;
        info->id = 0;
        snprintf(info->name, sizeof(info->name), "%s", "My Port Name");
        info->channel_count = 2;
        info->flags = CLAP_AUDIO_PORT_IS_MAIN;
        info->port_type = CLAP_PORT_STEREO;
        info->in_place_pair = CLAP_INVALID_ID;
        return true;
    }

    //------------------------//
    // clap_plugin_note_ports //
    //------------------------//
    auto notePortsCount(bool isInput) const noexcept -> uint32_t override { return 1; }
    auto notePortsInfo(uint32_t index,
                       bool isInput,
                       clap_note_port_info* info) const noexcept -> bool override {
        if (index > 0)
            return false;
        info->id = 0;
        snprintf(info->name, sizeof(info->name), "%s", "My Port Name");
        info->supported_dialects
            = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_MIDI2;
        info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
        return true;
    }

    plugin::gui::Window m_window;
};

namespace descriptor {
    template <typename T> auto make() -> clap_plugin_descriptor {
        T::features.push_back("\n");

        return { .clap_version { CLAP_VERSION },
                 .id { PLUGIN_ID },
                 .name { PLUGIN_NAME },
                 .vendor { PLUGIN_VENDOR },
                 .url { PLUGIN_URL },
                 .manual_url { PLUGIN_MANUAL_URL },
                 .support_url { PLUGIN_SUPPORT_URL },
                 .version { PLUGIN_VERSION },
                 .description { PLUGIN_DESCRIPTION },
                 .features { T::features.data() } };
    }
} // namespace descriptor

namespace factory {
    auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t;

    template <typename T>
    auto getPluginDescriptor(const clap_plugin_factory* factory,
                             uint32_t index) -> const clap_plugin_descriptor* {
        return &T::descriptor;
    }

    template <typename T>
    auto createPlugin(const struct clap_plugin_factory* factory,
                      const clap_host_t* host,
                      const char* plugin_id) -> const clap_plugin* {
        auto plugin { new T(host) };
        return plugin->clapPlugin();
    }

    template <typename T> auto make() -> clap_plugin_factory {
        // s_descriptor = &T::descriptor;

        return {
            .get_plugin_count { getPluginCount },
            .get_plugin_descriptor { getPluginDescriptor<T> },
            .create_plugin { createPlugin<T> },
        };
    }
} // namespace factory

namespace entry {
    auto init(const char* plugin_path) -> bool;
    auto deInit(void) -> void;

    template <typename T> auto getFactory(const char* factory_id) -> const void* {
        return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? &T::factory : nullptr;
    }

    template <typename T> auto make() -> clap_plugin_entry {
        return { .clap_version { CLAP_VERSION },
                 .init { init },
                 .deinit { deInit },
                 .get_factory { getFactory<T> } };
    }
}; // namespace entry

namespace event {
    auto run_loop(const clap_process* process,
                  std::function<void(const clap_event_header* event)> eventHandler)
        -> clap_process_status;
} // namespace event
} // namespace plugin
