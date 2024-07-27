#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <array>
#include <functional>
#include <initializer_list>

using PluginHelper = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                           clap::helpers::CheckingLevel::Maximal>;

template <typename T> constexpr auto makePluginDescriptor(T features) -> clap_plugin_descriptor {
    return { .clap_version { CLAP_VERSION },
             .id { PLUGIN_ID },
             .name { PLUGIN_NAME },
             .vendor { PLUGIN_VENDOR },
             .url { PLUGIN_URL },
             .manual_url { PLUGIN_MANUAL_URL },
             .support_url { PLUGIN_SUPPORT_URL },
             .version { PLUGIN_VERSION },
             .description { PLUGIN_DESCRIPTION },
             .features { features } };
}

struct PluginFactory {
    inline static uint32_t count { 0 };
    inline static const clap_plugin_descriptor* descriptor { nullptr };

    static auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t {
        return getPluginCountCallback(factory);
    }

    static auto getPluginDescriptor(const clap_plugin_factory* factory,
                                    uint32_t index) -> const clap_plugin_descriptor* {
        return getPluginDescriptorCallback(factory, index);
    }

    static auto createPlugin(const struct clap_plugin_factory* factory,
                             const clap_host_t* host,
                             const char* plugin_id) -> const clap_plugin* {
        return createPluginCallback(factory, host, plugin_id);
    }

    inline static std::function<uint32_t(const clap_plugin_factory* factory)>
        getPluginCountCallback { [](const clap_plugin_factory* factory) { return count; } };

    inline static std::function<const clap_plugin_descriptor*(
        const struct clap_plugin_factory* factory, uint32_t index)>
        getPluginDescriptorCallback { [](const struct clap_plugin_factory* factory,
                                         uint32_t index) { return descriptor; } };

    inline static std::function<const clap_plugin*(
        const struct clap_plugin_factory* factory, const clap_host_t* host, const char* plugin_id)>
        createPluginCallback { [](const struct clap_plugin_factory* factory,
                                  const clap_host_t* host,
                                  const char* plugin_id) { return nullptr; } };
};

auto makePluginFactory(uint32_t count,
                       const clap_plugin_descriptor* descriptor,
                       std::function<const clap_plugin*(const struct clap_plugin_factory* factory,
                                                        const clap_host_t* host,
                                                        const char* plugin_id)> callback)
    -> clap_plugin_factory {
    PluginFactory::count = count;
    PluginFactory::descriptor = descriptor;
    PluginFactory::createPluginCallback = callback;

    clap_plugin_factory pluginFactory {
        .get_plugin_count { PluginFactory::getPluginCount },
        .get_plugin_descriptor { PluginFactory::getPluginDescriptor },
        .create_plugin { PluginFactory::createPlugin },
    };

    return pluginFactory;
}
