#pragma once

#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <algorithm>
#include <cstring>
#include <format>
#include <functional>
#include <iomanip>
#include <iostream>
#include <print>
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "gui/gui.hxx"

namespace plugin {
using ParameterToValue = std::unordered_map<clap_id, double*>;
using Features = std::vector<const char*>;
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

struct Helper : public IgnoreNone {
    using IgnoreNone::IgnoreNone;

    //--------------------//
    // clap_plugin_params //
    //--------------------//
    clap_id nParams { 0 };
    plugin::ParameterToValue paramToValue;
    auto paramsCount() const noexcept -> uint32_t override { return nParams; }

    //-----------------//
    // clap_plugin_gui //
    //-----------------//
    auto guiIsApiSupported(const char* api, bool isFloating) noexcept -> bool override {
        if (isFloating) {
            return false;
        }

        if (WIN32) {
            if (std::strcmp(api, CLAP_WINDOW_API_WIN32) == 0) {
                return true;
            }
        }

        return false;
    }

    auto guiGetPreferredApi(const char** api, bool* is_floating) noexcept -> bool override {
        return false;
    }

    auto guiCreate(const char* api, bool isFloating) noexcept -> bool override {
        return plugin::gui::create();
    }

    auto guiDestroy() noexcept -> void override { plugin::gui::destroy(); }

    auto guiShow() noexcept -> bool override { return plugin::gui::show(); }

    auto guiHide() noexcept -> bool override { return plugin::gui::hide(); }

    auto guiSetScale(double scale) noexcept -> bool override {
        return plugin::gui::setScale(scale);
    }

    auto guiCanResize() const noexcept -> bool override { return true; }

    auto guiSetSize(uint32_t width, uint32_t height) noexcept -> bool override {
        plugin::gui::setSize(width, height);

        return true;
    }

    auto guiGetSize(uint32_t* width, uint32_t* height) noexcept -> bool override {
        // std::cout << "getSize: " << width << " x " << height << std::endl;
        // *width = 200;
        // *height = 200;

        // return true;

        return false;
    }

    auto guiSetParent(const clap_window* window) noexcept -> bool override {
        return plugin::gui::setParent(window);
    }

    // virtual bool guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept { return false; }
    // virtual bool guiAdjustSize(uint32_t* width, uint32_t* height) noexcept {
    //     return guiGetSize(width, height);
    // }
    // virtual void guiSuggestTitle(const char* title) noexcept { }
    // virtual bool guiSetTransient(const clap_window* window) noexcept { return false; }
};
} // namespace plugin

namespace plugin::descriptor {
auto make(plugin::Features& features) -> clap_plugin_descriptor {
    features.push_back(nullptr);

    return { .clap_version { CLAP_VERSION },
             .id { PLUGIN_ID },
             .name { PLUGIN_NAME },
             .vendor { PLUGIN_VENDOR },
             .url { PLUGIN_URL },
             .manual_url { PLUGIN_MANUAL_URL },
             .support_url { PLUGIN_SUPPORT_URL },
             .version { PLUGIN_VERSION },
             .description { PLUGIN_DESCRIPTION },
             .features { features.data() } };
}
} // namespace plugin::descriptor

namespace plugin::factory {
const clap_plugin_descriptor* s_descriptor { nullptr };
std::function<const clap_plugin*(const clap_host_t* host)> s_callback {
    [](const clap_host_t* host) { return nullptr; }
};

auto getPluginCount(const clap_plugin_factory* factory) -> uint32_t { return 1; }

auto getPluginDescriptor(const clap_plugin_factory* factory,
                         uint32_t index) -> const clap_plugin_descriptor* {
    return s_descriptor;
}

auto createPlugin(const struct clap_plugin_factory* factory,
                  const clap_host_t* host,
                  const char* plugin_id) -> const clap_plugin* {
    return s_callback(host);
}

auto make(const clap_plugin_descriptor* descriptor,
          std::function<const clap_plugin*(const clap_host_t* host)> callback)
    -> clap_plugin_factory {
    s_descriptor = descriptor;
    s_callback = callback;

    return {
        .get_plugin_count { getPluginCount },
        .get_plugin_descriptor { getPluginDescriptor },
        .create_plugin { createPlugin },
    };
}
} // namespace plugin::factory

namespace plugin::entry {
const clap_plugin_factory* s_factory;

auto init(const char* plugin_path) -> bool { return true; }

auto deInit(void) -> void { }

auto getFactory(const char* factory_id) -> const void* {
    return (factory_id != CLAP_PLUGIN_FACTORY_ID) ? s_factory : nullptr;
}

auto make(const clap_plugin_factory* factory) -> clap_plugin_entry {
    s_factory = factory;

    return { .clap_version { CLAP_VERSION },
             .init { init },
             .deinit { deInit },
             .get_factory { getFactory } };
}
}; // namespace plugin::entry

namespace plugin::event {
//
auto run_loop(const clap_process* process,
              std::function<void(const clap_event_header* event)> eventHandler)
    -> clap_process_status {
    if (process->audio_outputs_count <= 0) {
        return CLAP_PROCESS_SLEEP;
    }

    auto ev { process->in_events };
    auto sz { ev->size(ev) };

    const clap_event_header* nextEvent { nullptr };
    clap_id nextEventIndex { 0 };
    if (sz != 0) {
        nextEvent = ev->get(ev, nextEventIndex);
    }

    for (uint32_t i { 0 }; i < process->frames_count; ++i) {
        while (nextEvent && nextEvent->time == i) {
            eventHandler(nextEvent);
            nextEventIndex++;
            if (nextEventIndex >= sz) {
                nextEvent = nullptr;
            } else {
                nextEvent = ev->get(ev, nextEventIndex);
            }
        }
    }

    assert(!nextEvent);

    return CLAP_PROCESS_SLEEP;
}
} // namespace plugin::event
