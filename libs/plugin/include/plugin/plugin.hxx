#pragma once

#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <functional>
#include <unordered_map>
#include <vector>

namespace plugin {
using TerminateMax = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                           clap::helpers::CheckingLevel::Maximal>;
using TerminateMin = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                           clap::helpers::CheckingLevel::Minimal>;
using TerminateNone = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                            clap::helpers::CheckingLevel::None>;
using IgnoreMax = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                        clap::helpers::CheckingLevel::Maximal>;
using IgnoreMin = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                        clap::helpers::CheckingLevel::Minimal>;
using IgnoreNone = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                         clap::helpers::CheckingLevel::None>;
using ParameterToValue = std::unordered_map<clap_id, double*>;
using Features = std::vector<const char*>;

struct Helper : public IgnoreNone {
    using IgnoreNone::IgnoreNone;

    //--------------------//
    // clap_plugin_params //
    //--------------------//
    clap_id nParams { 0 };
    plugin::ParameterToValue paramToValue;
    auto paramsCount() const noexcept -> uint32_t override;

    //-----------------//
    // clap_plugin_gui //
    //-----------------//
    auto guiIsApiSupported(const char* api, bool isFloating) noexcept -> bool override;
    // auto guiGetPreferredApi(const char** api, bool* is_floating) noexcept -> bool override;
    // auto guiCreate(const char* api, bool isFloating) noexcept -> bool override;
    // auto guiDestroy() noexcept -> void override;
    // auto guiShow() noexcept -> bool override;
    // auto guiHide() noexcept -> bool override;
    // auto guiSetScale(double scale) noexcept -> bool override;
    // auto guiCanResize() const noexcept -> bool override;
    // auto guiSetSize(uint32_t width, uint32_t height) noexcept -> bool override;
    // auto guiGetSize(uint32_t* width, uint32_t* height) noexcept -> bool override;
    // auto guiSetParent(const clap_window* window) noexcept -> bool override;
    // auto guiAdjustSize(uint32_t* width, uint32_t* height) noexcept -> bool override;

    // auto guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept -> bool override;
    // auto guiSuggestTitle(const char* title) noexcept -> void override;
    // auto guiSetTransient(const clap_window* window) noexcept -> bool override;

    //-------------------------//
    // clap_plugin_audio_ports //
    //-------------------------//
    auto audioPortsCount(bool isInput) const noexcept -> uint32_t override;
    auto audioPortsInfo(uint32_t index,
                        bool isInput,
                        clap_audio_port_info* info) const noexcept -> bool override;

    //------------------------//
    // clap_plugin_note_ports //
    //------------------------//
    auto notePortsCount(bool isInput) const noexcept -> uint32_t override;
    auto notePortsInfo(uint32_t index,
                       bool isInput,
                       clap_note_port_info* info) const noexcept -> bool override;
};

namespace descriptor {
    template <typename T> auto make() -> clap_plugin_descriptor {
        T::features.push_back("\n");

        return { .clap_version { CLAP_VERSION },
                 .id { PLUGIN_ID },
                 .name { PLUGIN_NAME },
                 .vendor { PLUGIN_VENDOR },
                 .url { PLUGIN_URL },
                 .manual_url { PLUGIN_MANUAL_URL },
                 .support_url { PLUGIN_SUPPORT_URL },
                 .version { PLUGIN_VERSION },
                 .description { PLUGIN_DESCRIPTION },
                 .features { T::features.data() } };
    }
} // namespace descriptor

namespace factory {
    auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t;

    template <typename T>
    auto getPluginDescriptor(const clap_plugin_factory* factory,
                             uint32_t index) -> const clap_plugin_descriptor* {
        return &T::descriptor;
    }

    template <typename T>
    auto createPlugin(const struct clap_plugin_factory* factory,
                      const clap_host_t* host,
                      const char* plugin_id) -> const clap_plugin* {
        auto plugin { new T(host) };
        return plugin->clapPlugin();
    }

    template <typename T> auto make() -> clap_plugin_factory {
        // s_descriptor = &T::descriptor;

        return {
            .get_plugin_count { getPluginCount },
            .get_plugin_descriptor { getPluginDescriptor<T> },
            .create_plugin { createPlugin<T> },
        };
    }
} // namespace factory

namespace entry {
    auto init(const char* plugin_path) -> bool;
    auto deInit(void) -> void;

    template <typename T> auto getFactory(const char* factory_id) -> const void* {
        return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? &T::factory : nullptr;
    }

    template <typename T> auto make() -> clap_plugin_entry {
        return { .clap_version { CLAP_VERSION },
                 .init { init },
                 .deinit { deInit },
                 .get_factory { getFactory<T> } };
    }
}; // namespace entry

namespace event {
    auto run_loop(const clap_process* process,
                  std::function<void(const clap_event_header* event)> eventHandler)
        -> clap_process_status;
} // namespace event
} // namespace plugin
