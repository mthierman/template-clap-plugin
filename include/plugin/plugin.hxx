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
    inline static uint32_t s_count { 0 };
    inline static const clap_plugin_descriptor* s_descriptor { nullptr };

    static auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t {
        return s_getPluginCountCallback(factory);
    }

    static auto getPluginDescriptor(const clap_plugin_factory* factory,
                                    uint32_t index) -> const clap_plugin_descriptor* {
        return s_getPluginDescriptorCallback(factory, index);
    }

    static auto createPlugin(const struct clap_plugin_factory* factory,
                             const clap_host_t* host,
                             const char* plugin_id) -> const clap_plugin* {
        return s_createPluginCallback(host);
    }

    inline static std::function<uint32_t(const clap_plugin_factory* factory)>
        s_getPluginCountCallback { [](const clap_plugin_factory* factory) { return s_count; } };

    inline static std::function<const clap_plugin_descriptor*(
        const struct clap_plugin_factory* factory, uint32_t index)>
        s_getPluginDescriptorCallback { [](const struct clap_plugin_factory* factory,
                                           uint32_t index) { return s_descriptor; } };

    inline static std::function<const clap_plugin*(const clap_host_t* host)>
        s_createPluginCallback { [](const clap_host_t* host) { return nullptr; } };

    static auto
    make(uint32_t count,
         const clap_plugin_descriptor* descriptor,
         std::function<const clap_plugin*(const clap_host_t* host)> createPluginCallback)
        -> const clap_plugin_factory {
        PluginFactory::s_count = count;
        PluginFactory::s_descriptor = descriptor;
        PluginFactory::s_createPluginCallback = createPluginCallback;

        clap_plugin_factory pluginFactory {
            .get_plugin_count { PluginFactory::getPluginCount },
            .get_plugin_descriptor { PluginFactory::getPluginDescriptor },
            .create_plugin { PluginFactory::createPlugin },
        };

        return pluginFactory;
    }
};

struct PluginEntry {
    inline static const clap_plugin_factory* s_factory;

    static auto make(const clap_plugin_factory* factory) -> const clap_plugin_entry {
        PluginEntry::s_factory = factory;

        clap_plugin_entry pluginEntry { .clap_version { CLAP_VERSION },
                                        .init {
                                            [](const char* plugin_path) -> bool { return true; } },
                                        .deinit { [](void) -> void {} },
                                        .get_factory { [](const char* factory_id) -> const void* {
            return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? PluginEntry::s_factory : nullptr;
        } } };

        return pluginEntry;
    }
};
