#include "../include/plugin/plugin.hxx"

#include <cassert>

namespace plugin {
namespace descriptor {
    auto make(Features& features) -> clap_plugin_descriptor {
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
    auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t { return 1; }

    auto getPluginDescriptor(const clap_plugin_factory* factory,
                             uint32_t index) -> const clap_plugin_descriptor* {
        return s_descriptor;
    }
} // namespace factory

namespace entry {
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
