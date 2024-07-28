#include "../include/plugin/plugin.hxx"

namespace plugin {
//--------------------//
// clap_plugin_params //
//--------------------//
auto Helper::paramsCount() const noexcept -> uint32_t { return nParams; }

//-------------------------//
// clap_plugin_audio_ports //
//-------------------------//
auto Helper::audioPortsCount(bool isInput) const noexcept -> uint32_t { return 1; }
auto Helper::audioPortsInfo(uint32_t index,
                            bool isInput,
                            clap_audio_port_info* info) const noexcept -> bool {
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
auto Helper::notePortsCount(bool isInput) const noexcept -> uint32_t { return 1; }
auto Helper::notePortsInfo(uint32_t index,
                           bool isInput,
                           clap_note_port_info* info) const noexcept -> bool {
    if (index > 0)
        return false;
    info->id = 0;
    snprintf(info->name, sizeof(info->name), "%s", "My Port Name");
    info->supported_dialects
        = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_MIDI2;
    info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
    return true;
}

namespace descriptor {
    auto make(plugin::Features& features) -> clap_plugin_descriptor {
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
} // namespace descriptor

namespace factory {
    const clap_plugin_descriptor* s_descriptor { nullptr };

    std::function<const clap_plugin*(const clap_host_t* host)> s_callback {
        [](const clap_host_t* host) { return nullptr; }
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

    auto make(const clap_plugin_descriptor* descriptor,
              std::function<const clap_plugin*(const clap_host_t* host)> callback)
        -> clap_plugin_factory {
        s_descriptor = descriptor;
        s_callback = callback;

        return {
            .get_plugin_count { getPluginCount },
            .get_plugin_descriptor { getPluginDescriptor },
            .create_plugin { createPlugin },
        };
    }
} // namespace factory

namespace entry {
    const clap_plugin_factory* s_factory;

    auto init(const char* plugin_path) -> bool { return true; }

    auto deInit(void) -> void { }

    auto getFactory(const char* factory_id) -> const void* {
        return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? s_factory : nullptr;
    }

    auto make(const clap_plugin_factory* factory) -> clap_plugin_entry {
        s_factory = factory;

        return { .clap_version { CLAP_VERSION },
                 .init { init },
                 .deinit { deInit },
                 .get_factory { getFactory } };
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
