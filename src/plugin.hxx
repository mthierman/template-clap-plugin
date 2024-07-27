#include <functional>

#include "gain.hxx"

namespace plugin {
static std::function<uint32_t(const clap_plugin_factory* factory)> getPluginCount {
    [](const struct clap_plugin_factory* factory) -> uint32_t { return 1; }
};

std::function<const clap_plugin_descriptor*(const struct clap_plugin_factory* factory,
                                            uint32_t index)>
    getPluginDescriptor { [](const struct clap_plugin_factory* factory, uint32_t index) {
    return &plugin::Gain::plugin_descriptor;
} };

std::function<const clap_plugin*(
    const struct clap_plugin_factory* factory, const clap_host_t* host, const char* plugin_id)>
    createPlugin { [](const struct clap_plugin_factory* factory,
                      const clap_host_t* host,
                      const char* plugin_id) -> const clap_plugin* {
    auto p = new plugin::Gain(host);
    return p->clapPlugin();
} };

extern "C" {
const CLAP_EXPORT clap_plugin_factory plugin_factory {
    .get_plugin_count {
        [](const clap_plugin_factory* factory) -> uint32_t { return getPluginCount(factory); } },

    .get_plugin_descriptor { [](const struct clap_plugin_factory* factory,
                                uint32_t index) -> const clap_plugin_descriptor* {
    return getPluginDescriptor(factory, index);
} },

    .create_plugin { [](const struct clap_plugin_factory* factory,
                        const clap_host_t* host,
                        const char* plugin_id) -> const clap_plugin* {
    return createPlugin(factory, host, plugin_id);
} },
};
}

auto makeEntry() -> clap_plugin_entry {
    return { .clap_version { CLAP_VERSION },

             .init { [](const char* plugin_path) -> bool { return true; } },

             .deinit { [](void) -> void {} },

             .get_factory { [](const char* factory_id) -> const void* {
        return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? &plugin_factory : nullptr;
    } } };
}
} // namespace plugin
