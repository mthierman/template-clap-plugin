#pragma once

#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <functional>
#include <unordered_map>
#include <vector>

#include "config.hxx"
#include "window.hxx"

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

template <typename T, typename U> struct PluginHelper : public U {
    using U::U;

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

    auto guiCreate(const char* api, bool isFloating) noexcept -> bool override { return true; }

    auto guiSetScale(double scale) noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            m_window.m_scale = scale;

            return true;
        }

        return false;
    }

    auto guiCanResize() const noexcept -> bool override { return true; }

    auto guiAdjustSize(uint32_t* width, uint32_t* height) noexcept -> bool override {
        return guiSetSize(*width, *height);
    }

    auto guiSetSize(uint32_t width, uint32_t height) noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            glow::messages::send_message(
                m_window.m_hwnd.get(), WM_NOTIFY, WM_APP, MAKELPARAM(width, height));

            return true;
        }

        return true;
    }

    auto guiGetSize(uint32_t* width, uint32_t* height) noexcept -> bool override { return true; }

    auto guiSetParent(const clap_window* window) noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            ::SetWindowLongPtrA(m_window.m_hwnd.get(), GWL_STYLE, WS_POPUP);
            glow::window::set_parent(m_window.m_hwnd.get(), (::HWND)window->win32);

            return true;
        }

        return false;
    }

    auto guiShow() noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            glow::window::show(m_window.m_hwnd.get());

            return true;
        }

        return false;
    }

    auto guiHide() noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            glow::window::hide(m_window.m_hwnd.get());

            return true;
        }

        return false;
    }

    auto guiDestroy() noexcept -> void override { m_window.m_hwnd.reset(); }

    auto guiGetPreferredApi(const char** api, bool* is_floating) noexcept -> bool override {
        return false;
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

    plugin::Window m_window;
    plugin::Config m_config;
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
    template <typename T> auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t {
        return 1;
    }

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
            .get_plugin_count { getPluginCount<T> },
            .get_plugin_descriptor { getPluginDescriptor<T> },
            .create_plugin { createPlugin<T> },
        };
    }
} // namespace factory

namespace entry {
    template <typename T> auto init(const char* plugin_path) -> bool { return true; }

    template <typename T> auto deInit(void) -> void { }

    template <typename T> auto getFactory(const char* factory_id) -> const void* {
        return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? &T::factory : nullptr;
    }

    template <typename T> auto make() -> clap_plugin_entry {
        return { .clap_version { CLAP_VERSION },
                 .init { init<T> },
                 .deinit { deInit<T> },
                 .get_factory { getFactory<T> } };
    }
}; // namespace entry

namespace event {
    auto run_loop(const clap_process* process,
                  std::function<void(const clap_event_header* event)> eventHandler)
        -> clap_process_status {
        if (process->audio_outputs_count <= 0) {
            return CLAP_PROCESS_SLEEP;
        }

        auto ev { process->in_events };
        auto sz { ev->size(ev) };

        const clap_event_header* nextEvent { nullptr };
        clap_id nextEventIndex { 0 };
        if (sz != 0) {
            nextEvent = ev->get(ev, nextEventIndex);
        }

        for (uint32_t i { 0 }; i < process->frames_count; ++i) {
            while (nextEvent && nextEvent->time == i) {
                eventHandler(nextEvent);
                nextEventIndex++;
                if (nextEventIndex >= sz) {
                    nextEvent = nullptr;
                } else {
                    nextEvent = ev->get(ev, nextEventIndex);
                }
            }
        }

        assert(!nextEvent);

        return CLAP_PROCESS_SLEEP;
    }
} // namespace event
} // namespace plugin
