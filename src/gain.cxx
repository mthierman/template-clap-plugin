#include "plugin.hxx"

extern "C" {
const CLAP_EXPORT clap_plugin_entry clap_entry { plugin::make() };
}
