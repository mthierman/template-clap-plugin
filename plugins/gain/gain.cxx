#include <plugin/plugin.hxx>

#include <sstream>
#include <string>

namespace plugins::gain {
struct Plugin final : public plugin::PluginHelper<Plugin> {
    explicit Plugin(const clap_host* host)
        : plugin::PluginHelper<Plugin>(&descriptor, host) {
        paramToValue[pmLevel] = &level;
        nParams = static_cast<clap_id>(paramToValue.size());
    }
    ~Plugin() { }

    // implements
    auto implementsGui() const noexcept -> bool override { return true; }
    auto implementsParams() const noexcept -> bool override { return true; }
    auto implementsAudioPorts() const noexcept -> bool override { return true; }
    auto implementsNotePorts() const noexcept -> bool override { return true; }

    // process
    auto process(const clap_process* process) noexcept -> clap_process_status override {
        return plugin::event::run_loop(
            process, [this](const clap_event_header_t* event) { handleEvent(event); });
    }

    // params
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

    // events
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

    enum paramIds : uint32_t { pmLevel };
    double level { 0.3 };
};

extern "C" {
const CLAP_EXPORT clap_plugin_entry clap_entry { Plugin::entry };
}
} // namespace plugins::gain
