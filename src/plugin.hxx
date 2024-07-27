#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <array>

namespace plugin {
auto make() -> bool { return true; }

struct Plugin : public clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                             clap::helpers::CheckingLevel::None> {
    Plugin(const clap_host* host)
        : clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                clap::helpers::CheckingLevel::None>(&plugin_descriptor, host) { }

    static constexpr std::array features { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
                                           CLAP_PLUGIN_FEATURE_UTILITY,
                                           "\0" };

    static constexpr clap_plugin_descriptor plugin_descriptor { .clap_version { CLAP_VERSION },
                                                                .id { PLUGIN_ID },
                                                                .name { PLUGIN_NAME },
                                                                .vendor { PLUGIN_VENDOR },
                                                                .url { PLUGIN_URL },
                                                                .manual_url { PLUGIN_MANUAL_URL },
                                                                .support_url { PLUGIN_SUPPORT_URL },
                                                                .version { PLUGIN_VERSION },
                                                                .description { PLUGIN_DESCRIPTION },
                                                                .features { features.data() } };

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
auto getPluginCount(const struct clap_plugin_factory* factory) -> uint32_t { return 1; }

auto getPluginDescriptor(const struct clap_plugin_factory* factory,
                         uint32_t index) -> const clap_plugin_descriptor* {
    return &Plugin::plugin_descriptor;
}

auto createPlugin(const struct clap_plugin_factory* factory,
                  const clap_host_t* host,
                  const char* plugin_id) -> const clap_plugin_t* {
    auto p = new Plugin(host);
    return p->clapPlugin();
}

const CLAP_EXPORT clap_plugin_factory factory {
    .get_plugin_count { plugin::getPluginCount },
    .get_plugin_descriptor { plugin::getPluginDescriptor },
    .create_plugin { plugin::createPlugin },
};

// clap_entry
auto init(const char* plugin_path) -> bool { return true; }
auto deInit(void) -> void { }
auto getFactory(const char* factory_id) -> const void* {
    return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? &factory : nullptr;
}
} // namespace plugin
