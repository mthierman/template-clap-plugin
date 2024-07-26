#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

namespace plugin {
struct Plugin : public clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                             clap::helpers::CheckingLevel::None> {
    Plugin(const clap_host* host)
        : clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                clap::helpers::CheckingLevel::None>(&plugin_descriptor, host) { }

    inline static const char* plugin_features[]
        = { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_UTILITY, NULL };

    inline static const clap_plugin_descriptor plugin_descriptor
        = { .clap_version { CLAP_VERSION },
            .id { "com.gmail.mthierman" },
            .name { "Plugin" },
            .vendor { "mthierman" },
            .url { "https://github.com/mthierman" },
            .manual_url { "https://github.com/mthierman" },
            .support_url { "https://github.com/mthierman" },
            .version { "0.0.0" },
            .description { "Plugin" },
            .features { plugin_features } };

    // clap_plugin_audio_ports
    virtual auto implementsAudioPorts() const noexcept -> bool { return true; }
    virtual auto audioPortsCount(bool isInput) const noexcept -> uint32_t { return 1; }
    virtual auto audioPortsInfo(uint32_t index,
                                bool isInput,
                                clap_audio_port_info* info) const noexcept -> bool {
        if (index > 0)
            return false;
        info->id = 0;
        snprintf(info->name, sizeof(info->name), "%s", "My Port Name");
        info->channel_count = 2;
        info->flags = CLAP_AUDIO_PORT_IS_MAIN;
        info->port_type = CLAP_PORT_STEREO;
        info->in_place_pair = CLAP_INVALID_ID;
        return true;
    }

    // clap_plugin_note_ports
    virtual auto implementsNotePorts() const noexcept -> bool { return true; }
    virtual auto notePortsCount(bool isInput) const noexcept -> uint32_t { return 1; }
    virtual auto
    notePortsInfo(uint32_t index, bool isInput, clap_note_port_info* info) const noexcept -> bool {
        if (index > 0)
            return false;
        info->id = 0;
        snprintf(info->name, sizeof(info->name), "%s", "My Port Name");
        info->supported_dialects
            = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_MIDI2;
        info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
        return true;
    }

    // clap_plugin
    virtual auto init() noexcept -> bool { return true; }
    virtual auto
    activate(double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept -> bool {
        return true;
    }
    virtual auto deactivate() noexcept -> void { }
    virtual auto startProcessing() noexcept -> bool { return true; }
    virtual auto stopProcessing() noexcept -> void { }
    virtual auto process(const clap_process* process) noexcept -> clap_process_status {
        return CLAP_PROCESS_SLEEP;
    }
    virtual auto reset() noexcept -> void { }
    virtual auto onMainThread() noexcept -> void { }
    virtual auto extension(const char* id) noexcept -> const void* { return nullptr; }
    virtual auto enableDraftExtensions() const noexcept -> bool { return false; }
};

// clap_plugin_factory
auto factory_get_plugin_count(const struct clap_plugin_factory* factory) -> uint32_t { return 1; }

auto factory_get_plugin_descriptor(const struct clap_plugin_factory* factory,
                                   uint32_t index) -> const clap_plugin_descriptor* {
    return &Plugin::plugin_descriptor;
}

auto factory_create_plugin(const struct clap_plugin_factory* factory,
                           const clap_host_t* host,
                           const char* plugin_id) -> const clap_plugin_t* {
    auto p = new Plugin(host);
    return p->clapPlugin();
}

const CLAP_EXPORT clap_plugin_factory clap_factory = {
    .get_plugin_count { plugin::factory_get_plugin_count },
    .get_plugin_descriptor { plugin::factory_get_plugin_descriptor },
    .create_plugin { plugin::factory_create_plugin },
};

// clap_entry
auto entry_init(const char* plugin_path) -> bool { return true; }
auto entry_deinit(void) -> void { }
auto entry_get_factory(const char* factory_id) -> const void* {
    return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? &clap_factory : nullptr;
}
} // namespace plugin

extern "C" {
const CLAP_EXPORT struct clap_plugin_entry clap_entry = {
    .clap_version { CLAP_VERSION },
    .init { plugin::entry_init },
    .deinit { plugin::entry_deinit },
    .get_factory { plugin::entry_get_factory },
};
}
