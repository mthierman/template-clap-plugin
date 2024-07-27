#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <array>
#include <initializer_list>

using PluginHelper = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                           clap::helpers::CheckingLevel::None>;

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
