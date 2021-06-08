#ifndef AUDIO_H
#define AUDIO_H
#define MAX_SAMPLES 8
#define MAX_CHANNELS 32
#include <AL/al.h>
#include <AL/alc.h>

#include <sndfile.h>

typedef struct {
    ALuint al_buffer;
} AudioSample_t;

AudioSample_t audiosample_LoadFile(const char* path);

struct AudioPlayer {
    AudioSample_t samples[MAX_SAMPLES];
    unsigned int channels[MAX_CHANNELS];
};
#endif