#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <array>
#include <functional>
#include <initializer_list>

namespace plugin {
using Helper = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                     clap::helpers::CheckingLevel::Maximal>;
} // namespace plugin

namespace plugin::descriptor {
template <typename T> auto make(T features) -> clap_plugin_descriptor {
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
} // namespace plugin::descriptor

namespace plugin::factory {
uint32_t pluginCount { 0 };
const clap_plugin_descriptor* pluginDescriptor { nullptr };

std::function<uint32_t(const clap_plugin_factory* factory)> getPluginCountCallback {
    [](const clap_plugin_factory* factory) { return pluginCount; }
};

std::function<const clap_plugin_descriptor*(const struct clap_plugin_factory* factory,
                                            uint32_t index)>
    getPluginDescriptorCallback { [](const struct clap_plugin_factory* factory, uint32_t index) {
    return pluginDescriptor;
} };

std::function<const clap_plugin*(const clap_host_t* host)> createPluginCallback {
    [](const clap_host_t* host) { return nullptr; }
};

auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t {
    return getPluginCountCallback(factory);
}

auto getPluginDescriptor(const clap_plugin_factory* factory,
                         uint32_t index) -> const clap_plugin_descriptor* {
    return getPluginDescriptorCallback(factory, index);
}

auto createPlugin(const struct clap_plugin_factory* factory,
                  const clap_host_t* host,
                  const char* plugin_id) -> const clap_plugin* {
    return createPluginCallback(host);
}

auto make(uint32_t count,
          const clap_plugin_descriptor* descriptor,
          std::function<const clap_plugin*(const clap_host_t* host)> callback)
    -> clap_plugin_factory {
    pluginCount = count;
    pluginDescriptor = descriptor;
    createPluginCallback = callback;

    return {
        .get_plugin_count { getPluginCount },
        .get_plugin_descriptor { getPluginDescriptor },
        .create_plugin { createPlugin },
    };
}
} // namespace plugin::factory

namespace plugin::entry {
const clap_plugin_factory* pluginFactory;

std::function<bool(const char* plugin_path)> s_init { [](const char* plugin_path) {
    return true;
} };

std::function<void()> s_deInit { []() {} };

std::function<const void*(const char* factory_id)> s_getFactory { [](const char* factory_id) {
    return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? pluginFactory : nullptr;
} };

auto init(const char* plugin_path) -> bool { return s_init(plugin_path); }

auto deInit(void) -> void { return s_deInit(); }

auto getFactory(const char* factory_id) -> const void* { return s_getFactory(factory_id); }

auto make(const clap_plugin_factory* factory) -> clap_plugin_entry {
    pluginFactory = factory;

    return { .clap_version { CLAP_VERSION },
             .init { init },
             .deinit { deInit },
             .get_factory { getFactory } };
}
}; // namespace plugin::entry
