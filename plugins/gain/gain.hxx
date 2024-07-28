#include <plugin/helper.hxx>
#include <plugin/plugin.hxx>

namespace gain {
struct Plugin final : public plugin::Helper {
    explicit Plugin(const clap_host* host)
        : plugin::Helper(&descriptor, host) {
        paramToValue[pmLevel] = &level;
        nParams = static_cast<clap_id>(paramToValue.size());
    }
    ~Plugin() { }

    //-----------------//
    // clap_descriptor //
    //-----------------//
    inline static plugin::Features features { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
                                              CLAP_PLUGIN_FEATURE_UTILITY };
    inline static const auto descriptor { plugin::descriptor::make(features) };

    //-------------//
    // clap_plugin //
    //-------------//
    auto process(const clap_process* process) noexcept -> clap_process_status override {
        return plugin::event::run_loop(
            process, [this](const clap_event_header_t* event) { handleEvent(event); });
    }

    //--------------------//
    // clap_plugin_params //
    //--------------------//
    enum paramIds : uint32_t { pmLevel };
    double level { 0.3 };

    auto implementsParams() const noexcept -> bool override { return true; }

    auto handleEvent(const clap_event_header_t* event) -> void {
        if (event->space_id != CLAP_CORE_EVENT_SPACE_ID) {
            return;
        }

        switch (event->type) {
            case CLAP_EVENT_PARAM_VALUE: {
                auto paramValue { reinterpret_cast<const clap_event_param_value*>(event) };
                *paramToValue[paramValue->param_id] = paramValue->value;
            } break;
        }
    }

    auto paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept -> bool override {
        if (paramIndex >= nParams) {
            return false;
        }

        info->flags = CLAP_PARAM_IS_AUTOMATABLE;

        switch (paramIndex) {
            case 0: {
                info->id = pmLevel;
                strcpy_s(info->name, CLAP_NAME_SIZE, "Level");
                strcpy_s(info->module, CLAP_NAME_SIZE, "Gain");
                info->min_value = 0.0;
                info->max_value = 1.0;
                info->default_value = 0.5;
            } break;
        }

        return true;
    }

    auto paramsValue(clap_id paramId, double* value) noexcept -> bool override {
        *value = *paramToValue[paramId];

        return true;
    }

    auto paramsValueToText(clap_id paramId,
                           double value,
                           char* display,
                           uint32_t size) noexcept -> bool override {
        auto id { static_cast<paramIds>(paramId) };
        std::string fmtString;

        switch (id) {
            case pmLevel: {
                fmtString = std::format("{}", value);
            } break;
        }

        strcpy_s(display, size, fmtString.c_str());

        return true;
    }

    auto paramsTextToValue(clap_id paramId,
                           const char* display,
                           double* value) noexcept -> bool override {
        auto id { static_cast<paramIds>(paramId) };

        switch (id) {
            case pmLevel: {
                *value = std::clamp(std::stod(display), 0.0, 1.0);
            } break;
        }

        return true;
    }

    auto paramsFlush(const clap_input_events* in,
                     const clap_output_events* out) noexcept -> void override {

        auto size { in->size(in) };

        for (auto e = 0U; e < size; ++e) {
            auto nextEvent { in->get(in, e) };
            handleEvent(nextEvent);
        }
    }

    auto isValidParamId(clap_id paramId) const noexcept -> bool override {
        return paramToValue.find(paramId) != paramToValue.end();
    }

    //-----------------//
    // clap_plugin_gui //
    //-----------------//
    // auto implementsGui() const noexcept -> bool override { return false; }

    //-------------------------//
    // clap_plugin_audio_ports //
    //-------------------------//
    // auto implementsAudioPorts() const noexcept -> bool override { return true; }

    //------------------------//
    // clap_plugin_note_ports //
    //------------------------//
    // auto implementsNotePorts() const noexcept -> bool override { return true; }
};

const auto pluginFactory { plugin::factory::make(&gain::Plugin::descriptor,
                                                 [](const clap_host_t* host) {
    auto plugin { new Plugin(host) };
    return plugin->clapPlugin();
}) };

const auto pluginEntry { plugin::entry::make(&pluginFactory) };
} // namespace gain
