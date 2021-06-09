#ifndef AUDIO_H
#define AUDIO_H
#define MAX_SAMPLES 8
#define MAX_CHANNELS 10
#include <AL/al.h>
#include <AL/alc.h>
#include <stdbool.h>

typedef struct {
	ALuint al_buffer;
} AudioSample_t;

AudioSample_t audiosample_LoadFile(const char* path);

typedef struct {
	unsigned int al_source;
	bool persistant;
} AudioChannel_t;

struct AudioPlayer {
	ALCdevice* device;
	ALCcontext* context;
	AudioSample_t samples[MAX_SAMPLES];
	AudioChannel_t channels[MAX_CHANNELS];
};

struct AudioPlayer audioplayer_Init();
unsigned int audioplayer_PlaySound(struct AudioPlayer to_play_from, unsigned int sample, bool persistant, bool looping);
void audioplayer_StopSound(struct AudioPlayer to_play_from, unsigned int channel);
unsigned int audioplayer_LoadSample(struct AudioPlayer* to_load_to, const char* path, unsigned int slot);
#endif