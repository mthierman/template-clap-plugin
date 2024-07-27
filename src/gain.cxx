#include "gain.hxx"

extern "C" {
const CLAP_EXPORT clap_plugin_entry clap_entry { gain::getPluginEntry() };
}
