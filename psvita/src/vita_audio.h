#pragma once

#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

int  vita_audio_init();
void vita_audio_shutdown();

int  vita_audio_open_channel(int samples, int freq, int fmt);
void vita_audio_close_channel(int channel);
int  vita_audio_output_blocking(int channel, int vol, const void *buf);
int  vita_audio_set_volume(int channel, int vol_left, int vol_right);

int  vita_audio_get_sample_count();
int  vita_audio_get_freq();

#ifdef __cplusplus
}
#endif
