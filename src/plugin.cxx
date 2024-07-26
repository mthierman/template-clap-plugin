#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include <clap/clap.h>

#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

const char* features[] = { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_UTILITY, NULL };

static const clap_plugin_descriptor s_my_plug_desc
    = { .clap_version { CLAP_VERSION },
        .id { "com.gmail.mthierman" },
        .name { "ClapPlugin" },
        .vendor { "mthierman" },
        .url { "https://github.com/mthierman" },
        .manual_url { "https://github.com/mthierman" },
        .support_url { "https://github.com/mthierman" },
        .version { "0.0.0" },
        .description { "A test CLAP plugin." },
        .features { features } };

struct Plugin : public clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                             clap::helpers::CheckingLevel::Maximal> {
    Plugin(const clap_plugin_descriptor* desc, const clap_host* host)
        : clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                clap::helpers::CheckingLevel::Maximal>(desc, host) { }

    //-------------------------//
    // clap_plugin_audio_ports //
    //-------------------------//
    virtual bool implementsAudioPorts() const noexcept { return true; }
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

    //------------------------//
    // clap_plugin_note_ports //
    //------------------------//
    virtual bool implementsNotePorts() const noexcept { return true; }
    virtual uint32_t notePortsCount(bool isInput) const noexcept { return 1; }
    virtual bool
    notePortsInfo(uint32_t index, bool isInput, clap_note_port_info* info) const noexcept {
        if (index > 0)
            return false;
        info->id = 0;
        snprintf(info->name, sizeof(info->name), "%s", "My Port Name");
        info->supported_dialects
            = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_MIDI2;
        info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
        return true;
    }

    //---------------------//
    // clap_plugin_latency //
    //---------------------//
    virtual bool implementsLatency() const noexcept { return false; }
    // virtual uint32_t latencyGet() const noexcept {
    //     my_plug_t* plug = (my_plug_t*)plugin->plugin_data;
    //     return plug->latency;
    // }

    //-------------------//
    // clap_plugin_state //
    //-------------------//
    virtual bool implementsState() const noexcept { return false; }
    // virtual bool stateSave(const clap_ostream* stream) noexcept {
    //     my_plug_t* plug = (my_plug_t*)plugin->plugin_data;
    //     // TODO: write the state into stream
    //     return true;
    // }
    // virtual bool stateLoad(const clap_istream* stream) noexcept {
    //     my_plug_t* plug = (my_plug_t*)plugin->plugin_data;
    //     // TODO: read the state from stream
    //     return true;
    // }

    //-------------//
    // clap_plugin //
    //-------------//
    virtual bool init() noexcept { return true; }
    virtual bool
    activate(double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept {
        return true;
    }
    virtual void deactivate() noexcept { }
    virtual bool startProcessing() noexcept { return true; }
    virtual void stopProcessing() noexcept { }
    virtual clap_process_status process(const clap_process* process) noexcept {
        return CLAP_PROCESS_SLEEP;
    }
    virtual void reset() noexcept { }
    virtual void onMainThread() noexcept { }
    virtual const void* extension(const char* id) noexcept { return nullptr; }
    virtual bool enableDraftExtensions() const noexcept { return false; }
};

/////////////////////////
// clap_plugin_factory //
/////////////////////////

static uint32_t plugin_factory_get_plugin_count(const struct clap_plugin_factory* factory) {
    return 1;
}

static const clap_plugin_descriptor*
plugin_factory_get_plugin_descriptor(const struct clap_plugin_factory* factory, uint32_t index) {
    return s_plugins[index].desc;
}

static const clap_plugin_t* plugin_factory_create_plugin(const struct clap_plugin_factory* factory,
                                                         const clap_host_t* host,
                                                         const char* plugin_id) {
    if (!clap_version_is_compatible(host->clap_version)) {
        return NULL;
    }

    const int N = sizeof(s_plugins) / sizeof(s_plugins[0]);
    for (int i = 0; i < N; ++i)
        if (!strcmp(plugin_id, s_plugins[i].desc->id))
            return s_plugins[i].create(host);

    return NULL;
}

const CLAP_EXPORT clap_plugin_factory plugin_factory = {
    .get_plugin_count = plugin_factory_get_plugin_count,
    .get_plugin_descriptor = plugin_factory_get_plugin_descriptor,
    .create_plugin = plugin_factory_create_plugin,
};

////////////////
// clap_entry //
////////////////

static bool entry_init(const char* plugin_path) { return true; }
static void entry_deinit(void) { }
static const void* get_factory(const char* factory_id) {
    if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0)
        return &aw2c_factory;
    return 0;
}

// This symbol will be resolved by the host
const CLAP_EXPORT struct clap_plugin_entry clap_entry = {
    .clap_version = CLAP_VERSION_INIT,
    .init = entry_init,
    .deinit = entry_deinit,
    .get_factory = entry_get_factory,
};
