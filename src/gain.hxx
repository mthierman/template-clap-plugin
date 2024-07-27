#include <plugin/plugin.hxx>

namespace gain {
plugin::Features features { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_UTILITY };
const auto descriptor { plugin::descriptor::make(features) };

using Helper = plugin::TerminateMax;
struct Plugin final : public Helper {
    explicit Plugin(const clap_host* host)
        : Helper(&descriptor, host) { }

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

    // clap_plugin_params
    auto implementsParams() const noexcept -> bool override { return true; }
    auto paramsCount() const noexcept -> uint32_t override { return 1; }
    auto paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept -> bool override {
        info->flags = CLAP_PARAM_IS_AUTOMATABLE;
        switch (paramIndex) {
            case 0: {
                info->id = 0;
                strcpy_s(info->name, CLAP_NAME_SIZE, "Gain");
            }
        }
        return false;
    }
    auto paramsValue(clap_id paramId, double* value) noexcept -> bool override { return false; }
    auto paramsValueToText(clap_id paramId,
                           double value,
                           char* display,
                           uint32_t size) noexcept -> bool override {
        return false;
    }
    auto paramsTextToValue(clap_id paramId,
                           const char* display,
                           double* value) noexcept -> bool override {
        return false;
    }
    auto paramsFlush(const clap_input_events* in,
                     const clap_output_events* out) noexcept -> void override { }

    // clap_plugin_gui
    // virtual bool implementsGui() const noexcept { return false; }
    // virtual bool guiIsApiSupported(const char* api, bool isFloating) noexcept { return false; }
    // virtual bool guiGetPreferredApi(const char** api, bool* is_floating) noexcept { return false;
    // } virtual bool guiCreate(const char* api, bool isFloating) noexcept { return false; } virtual
    // void guiDestroy() noexcept { } virtual bool guiSetScale(double scale) noexcept { return
    // false; } virtual bool guiShow() noexcept { return false; } virtual bool guiHide() noexcept {
    // return false; } virtual bool guiGetSize(uint32_t* width, uint32_t* height) noexcept { return
    // false; } virtual bool guiCanResize() const noexcept { return false; } virtual bool
    // guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept { return false; } virtual bool
    // guiAdjustSize(uint32_t* width, uint32_t* height) noexcept {
    //     return guiGetSize(width, height);
    // }
    // virtual bool guiSetSize(uint32_t width, uint32_t height) noexcept { return false; }
    // virtual void guiSuggestTitle(const char* title) noexcept { }
    // virtual bool guiSetParent(const clap_window* window) noexcept { return false; }
    // virtual bool guiSetTransient(const clap_window* window) noexcept { return false; }
};

const auto factory { plugin::factory::make(&descriptor, [](const clap_host_t* host) {
    auto plugin { new Plugin(host) };
    return plugin->clapPlugin();
}) };

const auto entry { plugin::entry::make(&factory) };
} // namespace gain
