#include <plugin/plugin.hxx>

namespace gain {
using Helper = plugin::TerminateMax;
enum paramIds : uint32_t { pmLevel };

struct Plugin final : public Helper {
    explicit Plugin(const clap_host* host)
        : Helper(&descriptor, host) {
        paramToValue[pmLevel] = &level;
    }
    ~Plugin() { }

    inline static plugin::Features features { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
                                              CLAP_PLUGIN_FEATURE_UTILITY };
    inline static const auto descriptor { plugin::descriptor::make(features) };

    clap_id nParams { 1 };
    double level { 0.3 };
    plugin::ParameterToValue paramToValue;

    //-------------//
    // clap_plugin //
    //-------------//
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

    //--------------------//
    // clap_plugin_params //
    //--------------------//
    auto implementsParams() const noexcept -> bool override { return true; }
    auto paramsCount() const noexcept -> uint32_t override { return nParams; }
    auto paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept -> bool override {
        info->flags = CLAP_PARAM_IS_AUTOMATABLE;

        switch (paramIndex) {
            case 0: {
                info->id = 0;
                strcpy_s(info->name, CLAP_NAME_SIZE, "Level");
                strcpy_s(info->module, CLAP_NAME_SIZE, "Gain");
                info->min_value = 0.0;
                info->max_value = 1.0;
                // info->default_value = 0.5;
                // info->flags = CLAP_PARAM_IS_AUTOMATABLE;
                info->cookie = nullptr;
            } break;
        }

        return true;
    }
    auto paramsValue(clap_id paramId, double* value) noexcept -> bool override {
        switch (paramId) {
            case 0: {
                *value = level;
            }
        }

        return true;
    }
    auto paramsValueToText(clap_id paramId,
                           double value,
                           char* display,
                           uint32_t size) noexcept -> bool override {
        std::string stringValue;

        switch (paramId) {
            case 0: {
                stringValue = std::to_string(value);
            } break;
        }

        strcpy_s(display, size, stringValue.c_str());

        std::cout << display << std::endl;

        return true;
    }
    auto paramsTextToValue(clap_id paramId,
                           const char* display,
                           double* value) noexcept -> bool override {
        switch (paramId) {
            case 0: {
                *value = std::clamp(std::atof(display), 0.0, 1.0);
                return true;
            } break;
        }

        return false;
    }
    auto paramsFlush(const clap_input_events* in,
                     const clap_output_events* out) noexcept -> void override { }

    // This method is meant for implementing contract checking, it isn't part of CLAP.
    // The default implementation will be slow, so consider overriding it with a faster one.
    // Returns -1 if the parameter isn't found.
    // auto getParamIndexForParamId(clap_id paramId) const noexcept -> int32_t override { }
    auto isValidParamId(clap_id paramId) const noexcept -> bool override {
        return paramToValue.find(paramId) != paramToValue.end();
    }
    // auto getParamInfoForParamId(clap_id paramId,
    //                             clap_param_info* info) const noexcept -> bool override { }

    //-----------------//
    // clap_plugin_gui //
    //-----------------//
    virtual bool implementsGui() const noexcept { return false; }
    virtual bool guiIsApiSupported(const char* api, bool isFloating) noexcept { return false; }
    virtual bool guiGetPreferredApi(const char** api, bool* is_floating) noexcept { return false; }
    virtual bool guiCreate(const char* api, bool isFloating) noexcept { return false; }
    virtual void guiDestroy() noexcept { }
    virtual bool guiSetScale(double scale) noexcept { return false; }
    virtual bool guiShow() noexcept { return false; }
    virtual bool guiHide() noexcept { return false; }
    virtual bool guiGetSize(uint32_t* width, uint32_t* height) noexcept { return false; }
    virtual bool guiCanResize() const noexcept { return false; }
    virtual bool guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept { return false; }
    virtual bool guiAdjustSize(uint32_t* width, uint32_t* height) noexcept {
        return guiGetSize(width, height);
    }
    virtual bool guiSetSize(uint32_t width, uint32_t height) noexcept { return false; }
    virtual void guiSuggestTitle(const char* title) noexcept { }
    virtual bool guiSetParent(const clap_window* window) noexcept { return false; }
    virtual bool guiSetTransient(const clap_window* window) noexcept { return false; }

    //-------------------------//
    // clap_plugin_audio_ports //
    //-------------------------//
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

    //------------------------//
    // clap_plugin_note_ports //
    //------------------------//
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
};

const auto pluginFactory { plugin::factory::make(&gain::Plugin::descriptor,
                                                 [](const clap_host_t* host) {
    auto plugin { new Plugin(host) };
    return plugin->clapPlugin();
}) };

const auto pluginEntry { plugin::entry::make(&pluginFactory) };
} // namespace gain
