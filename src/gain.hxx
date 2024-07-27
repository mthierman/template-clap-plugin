#include <plugin/plugin.hxx>

namespace gain {
constexpr auto pluginFeatures { std::to_array(
    { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_UTILITY, "\0" }) };

constexpr clap_plugin_descriptor pluginDescriptor { .clap_version { CLAP_VERSION },
                                                    .id { PLUGIN_ID },
                                                    .name { PLUGIN_NAME },
                                                    .vendor { PLUGIN_VENDOR },
                                                    .url { PLUGIN_URL },
                                                    .manual_url { PLUGIN_MANUAL_URL },
                                                    .support_url { PLUGIN_SUPPORT_URL },
                                                    .version { PLUGIN_VERSION },
                                                    .description { PLUGIN_DESCRIPTION },
                                                    .features { pluginFeatures.data() } };

struct Gain final : public PluginHelper {
    explicit Gain(const clap_host* host)
        : PluginHelper(&pluginDescriptor, host) { }

    // clap_plugin_audio_ports
    auto implementsAudioPorts() const noexcept -> bool override { return true; }
    auto audioPortsCount(bool isInput) const noexcept -> uint32_t override { return 1; }
    auto audioPortsInfo(uint32_t index,
                        bool isInput,
                        clap_audio_port_info* info) const noexcept -> bool override {
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
    auto implementsNotePorts() const noexcept -> bool override { return true; }
    auto notePortsCount(bool isInput) const noexcept -> uint32_t override { return 1; }
    auto notePortsInfo(uint32_t index,
                       bool isInput,
                       clap_note_port_info* info) const noexcept -> bool override {
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
    auto init() noexcept -> bool override { return true; }
    auto activate(double sampleRate,
                  uint32_t minFrameCount,
                  uint32_t maxFrameCount) noexcept -> bool override {
        return true;
    }
    auto deactivate() noexcept -> void override { }
    auto startProcessing() noexcept -> bool override { return true; }
    auto stopProcessing() noexcept -> void override { }
    auto process(const clap_process* process) noexcept -> clap_process_status override {
        return CLAP_PROCESS_SLEEP;
    }
    auto reset() noexcept -> void override { }
    auto onMainThread() noexcept -> void override { }
    auto extension(const char* id) noexcept -> const void* override { return nullptr; }
    auto enableDraftExtensions() const noexcept -> bool override { return false; }
};

constexpr clap_plugin_factory pluginFactory {
    .get_plugin_count {
        [](const clap_plugin_factory* factory) -> uint32_t { return 1; },
    },
    .get_plugin_descriptor {
        [](const struct clap_plugin_factory* factory,
           uint32_t index) -> const clap_plugin_descriptor* { return &pluginDescriptor; },
    },
    .create_plugin { [](const struct clap_plugin_factory* factory,
                        const clap_host_t* host,
                        const char* plugin_id) -> const clap_plugin* {
    auto gain { new Gain(host) };
    return gain->clapPlugin();
} }
};

constexpr clap_plugin_entry pluginEntry { .clap_version { CLAP_VERSION },
                                          .init { [](const char* plugin_path) -> bool {
    return true;
} },
                                          .deinit { [](void) -> void {} },
                                          .get_factory { [](const char* factory_id) -> const void* {
    return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? &pluginFactory : nullptr;
} } };
} // namespace gain
