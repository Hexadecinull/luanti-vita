#include "vita_audio.h"

#include <psp2/audioout.h>
#include <psp2/audioin.h>
#include <psp2/kernel/threadmgr.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

static constexpr int MAX_CHANNELS    = 8;
static constexpr int DEFAULT_SAMPLES = 512;
static constexpr int DEFAULT_FREQ    = 44100;

struct AudioChannel {
    int  handle;
    bool open;
    int  vol_left;
    int  vol_right;
};

static AudioChannel g_channels[MAX_CHANNELS];
static bool         g_initialized = false;

extern "C" {

int vita_audio_init()
{
    if (g_initialized) return 0;

    for (int i = 0; i < MAX_CHANNELS; ++i) {
        g_channels[i].handle    = -1;
        g_channels[i].open      = false;
        g_channels[i].vol_left  = SCE_AUDIO_VOLUME_0DB;
        g_channels[i].vol_right = SCE_AUDIO_VOLUME_0DB;
    }

    g_initialized = true;
    return 0;
}

void vita_audio_shutdown()
{
    if (!g_initialized) return;

    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (g_channels[i].open && g_channels[i].handle >= 0) {
            sceAudioOutReleasePort(g_channels[i].handle);
            g_channels[i].handle = -1;
            g_channels[i].open   = false;
        }
    }

    g_initialized = false;
}

int vita_audio_open_channel(int samples, int freq, int fmt)
{
    if (!g_initialized) return -1;

    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (!g_channels[i].open) {
            int grain = (samples <= 0) ? DEFAULT_SAMPLES : samples;
            int rate  = (freq    <= 0) ? DEFAULT_FREQ    : freq;

            int handle = sceAudioOutOpenPort(
                SCE_AUDIO_OUT_PORT_TYPE_MAIN,
                grain,
                rate,
                SCE_AUDIO_OUT_MODE_STEREO
            );

            if (handle < 0) return handle;

            g_channels[i].handle    = handle;
            g_channels[i].open      = true;
            g_channels[i].vol_left  = SCE_AUDIO_VOLUME_0DB;
            g_channels[i].vol_right = SCE_AUDIO_VOLUME_0DB;

            int vols[2] = { SCE_AUDIO_VOLUME_0DB, SCE_AUDIO_VOLUME_0DB };
            sceAudioOutSetVolume(handle,
                static_cast<SceAudioOutChannelFlag>(
                    SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH),
                vols);

            return i;
        }
    }

    return -1;
}

void vita_audio_close_channel(int channel)
{
    if (!g_initialized) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    if (!g_channels[channel].open) return;

    sceAudioOutReleasePort(g_channels[channel].handle);
    g_channels[channel].handle = -1;
    g_channels[channel].open   = false;
}

int vita_audio_output_blocking(int channel, int vol, const void *buf)
{
    if (!g_initialized) return -1;
    if (channel < 0 || channel >= MAX_CHANNELS) return -1;
    if (!g_channels[channel].open) return -1;

    return sceAudioOutOutput(g_channels[channel].handle, buf);
}

int vita_audio_set_volume(int channel, int vol_left, int vol_right)
{
    if (!g_initialized) return -1;
    if (channel < 0 || channel >= MAX_CHANNELS) return -1;
    if (!g_channels[channel].open) return -1;

    g_channels[channel].vol_left  = vol_left;
    g_channels[channel].vol_right = vol_right;

    int vols[2] = { vol_left, vol_right };
    return sceAudioOutSetVolume(
        g_channels[channel].handle,
        static_cast<SceAudioOutChannelFlag>(
            SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH),
        vols
    );
}

int vita_audio_get_sample_count() { return DEFAULT_SAMPLES; }
int vita_audio_get_freq()         { return DEFAULT_FREQ;    }

}
