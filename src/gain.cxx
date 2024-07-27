#include "plugin.hxx"

extern "C" {
const CLAP_EXPORT clap_plugin_entry clap_entry {
    .clap_version { CLAP_VERSION },
    .init { plugin::init },
    .deinit { plugin::deInit },
    .get_factory { plugin::getFactory },
};
}
