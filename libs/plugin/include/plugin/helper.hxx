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
    // auto guiIsApiSupported(const char* api, bool isFloating) noexcept -> bool override;
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
    // auto audioPortsCount(bool isInput) const noexcept -> uint32_t override;
    // auto audioPortsInfo(uint32_t index,
    //                     bool isInput,
    //                     clap_audio_port_info* info) const noexcept -> bool override;

    //------------------------//
    // clap_plugin_note_ports //
    //------------------------//
    // auto notePortsCount(bool isInput) const noexcept -> uint32_t override;
    // auto notePortsInfo(uint32_t index,
    //                    bool isInput,
    //                    clap_note_port_info* info) const noexcept -> bool override;
};
} // namespace plugin
