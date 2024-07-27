#include "plugin.hxx"

extern "C" {
const CLAP_EXPORT clap_plugin_entry clap_entry {
    .clap_version { CLAP_VERSION },
    .init { plugin::entry_init },
    .deinit { plugin::entry_deinit },
    .get_factory { plugin::entry_get_factory },
};
}
