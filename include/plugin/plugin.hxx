#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <functional>
#include <vector>

namespace plugin {
using Features = std::vector<const char*>;
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
} // namespace plugin

namespace plugin::descriptor {
auto make(plugin::Features& features) -> clap_plugin_descriptor {
    features.push_back("\0");

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
} // namespace plugin::factory

namespace plugin::entry {
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
}; // namespace plugin::entry
