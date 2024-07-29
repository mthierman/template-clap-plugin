#pragma once

#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#if PLATFORM_WINDOWS
#include "win32.hxx"
#endif

namespace plugin {
using Helper = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                     clap::helpers::CheckingLevel::None>;
using ParameterToValue = std::unordered_map<clap_id, double*>;
using Features = std::vector<const char*>;
using Descriptor = clap_plugin_descriptor;
using Factory = clap_plugin_factory;
using Entry = clap_plugin_entry;

struct PluginHelper : public Helper {
    PluginHelper(const clap_plugin_descriptor* desc, const clap_host* host)
        : Helper(desc, host) { }

    // params
    auto paramsCount() const noexcept -> uint32_t override { return nParams; }

    // gui
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

    auto guiCreate(const char* api, bool isFloating) noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            m_window = std::make_unique<plugin::Window>();

            return true;
        }

        return false;
    }

    auto guiSetScale(double scale) noexcept -> bool override { return false; }

    auto guiCanResize() const noexcept -> bool override { return true; }

    auto guiAdjustSize(uint32_t* width, uint32_t* height) noexcept -> bool override {
        // return guiSetSize(*width, *height);
        return false;
    }

    auto guiSetSize(uint32_t width, uint32_t height) noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            glow::window::set_position(m_window->m_hwnd.get(), 0, 0, width, height);

            return true;
        }

        return true;
    }

    auto guiGetSize(uint32_t* width, uint32_t* height) noexcept -> bool override { return false; }

    auto guiSetParent(const clap_window* window) noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            glow::window::set_style(m_window->m_hwnd.get(), WS_CHILD);
            glow::window::set_parent(m_window->m_hwnd.get(), (::HWND)window->win32);

            return true;
        }

        return false;
    }

    auto guiShow() noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            glow::window::show(m_window->m_hwnd.get());

            return true;
        }

        return false;
    }

    auto guiHide() noexcept -> bool override {
        if (PLATFORM_WINDOWS) {
            glow::window::hide(m_window->m_hwnd.get());

            return true;
        }

        return false;
    }

    auto guiDestroy() noexcept -> void override { m_window.reset(); }

    auto guiGetPreferredApi(const char** api, bool* is_floating) noexcept -> bool override {
        return false;
    }

    // audio ports
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

    // note ports
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

    clap_id nParams { 0 };
    plugin::ParameterToValue paramToValue;
    std::unique_ptr<plugin::Window> m_window;
};

auto make_descriptor(plugin::Features& features) -> Descriptor {
    features.push_back(nullptr);

    return { .clap_version { CLAP_VERSION },
             .id { PLUGIN_ID },
             .name { PLUGIN_NAME },
             .vendor { PLUGIN_VENDOR },
             .url { PLUGIN_URL },
             .manual_url { PLUGIN_MANUAL_URL },
             .support_url { PLUGIN_SUPPORT_URL },
             .version { PLUGIN_VERSION },
             .description { PLUGIN_DESCRIPTION },
             .features { features.data() } };
}

namespace factory {
    const Descriptor* s_descriptor { nullptr };

    std::function<const clap_plugin*(const clap_host_t* host)> s_callback {
        [](const clap_host_t* host) -> const clap_plugin* { return nullptr; }
    };

    auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t { return 1; }

    auto getPluginDescriptor(const clap_plugin_factory* factory,
                             uint32_t index) -> const clap_plugin_descriptor* {
        return s_descriptor;
    }

    auto createPlugin(const struct clap_plugin_factory* factory,
                      const clap_host_t* host,
                      const char* plugin_id) -> const clap_plugin* {
        return s_callback(host);
    }
} // namespace factory

auto make_factory(const Descriptor* descriptor,
                  std::function<const clap_plugin*(const clap_host_t* host)> callback) -> Factory {
    factory::s_descriptor = descriptor;
    factory::s_callback = callback;

    return {
        .get_plugin_count { factory::getPluginCount },
        .get_plugin_descriptor { factory::getPluginDescriptor },
        .create_plugin { factory::createPlugin },
    };
}

namespace entry {
    const Factory* s_factory { nullptr };

    auto init(const char* plugin_path) -> bool { return true; }

    auto deInit(void) -> void { }

    auto getFactory(const char* factory_id) -> const void* {
        return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? s_factory : nullptr;
    }
}; // namespace entry

auto make_entry(const clap_plugin_factory* factory) -> clap_plugin_entry {
    entry::s_factory = factory;

    return { .clap_version { CLAP_VERSION },
             .init { entry::init },
             .deinit { entry::deInit },
             .get_factory { entry::getFactory } };
}

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
