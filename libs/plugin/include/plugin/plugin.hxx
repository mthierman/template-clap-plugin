#pragma once

#include <clap/all.h>

#include <functional>
#include <vector>

namespace plugin {
using Features = std::vector<const char*>;

namespace descriptor {
    auto make(plugin::Features& features) -> clap_plugin_descriptor;
} // namespace descriptor

namespace factory {
    inline const clap_plugin_descriptor* s_descriptor { nullptr };

    inline std::function<const clap_plugin*(const clap_host_t* host)> s_callback {
        [](const clap_host_t* host) { return nullptr; }
    };

    auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t;
    auto getPluginDescriptor(const clap_plugin_factory* factory,
                             uint32_t index) -> const clap_plugin_descriptor*;
    auto createPlugin(const struct clap_plugin_factory* factory,
                      const clap_host_t* host,
                      const char* plugin_id) -> const clap_plugin*;
    auto make(const clap_plugin_descriptor* descriptor,
              std::function<const clap_plugin*(const clap_host_t* host)> callback)
        -> clap_plugin_factory;
} // namespace factory

namespace entry {
    inline const clap_plugin_factory* s_factory;
    auto init(const char* plugin_path) -> bool;
    auto deInit(void) -> void;
    auto getFactory(const char* factory_id) -> const void*;
    auto make(const clap_plugin_factory* factory) -> clap_plugin_entry;
}; // namespace entry

namespace event {
    auto run_loop(const clap_process* process,
                  std::function<void(const clap_event_header* event)> eventHandler)
        -> clap_process_status;
} // namespace event
} // namespace plugin
