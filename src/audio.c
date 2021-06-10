#include <sndfile.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio.h"

static void
list_audio_devices(const ALCchar* devices) {
	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;
	printf("devices:\n");
	while(device && *device != '\0' && next && *next != '\0') {
		printf("%s\n", devices);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
}

AudioSample_t
audiosample_LoadFile(const char* path) {
	ALuint buffer;
	alGenBuffers(1, &buffer);

	SF_INFO sfinfo;
	SNDFILE* sndfile = sf_open(path, SFM_READ, &sfinfo);
	ALenum format    = AL_NONE;
	if(sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if(sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;

	short* membuf = malloc((sfinfo.frames * sfinfo.channels) * sizeof(short));

	sf_count_t num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if(num_frames < 1) {
		exit(-1);
	}

	ALsizei num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	AudioSample_t as = {.al_buffer = buffer};

	return as;
}

struct AudioPlayer
audioplayer_Init() {
	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	struct AudioPlayer ap;
	ap.device = alcOpenDevice(NULL);
	if(!ap.device) {
		exit(-1);
	}
	ap.context = alcCreateContext(ap.device, NULL);
	if(!alcMakeContextCurrent(ap.context)) {
		exit(-1);
	}

	for(int i = 0; i < MAX_CHANNELS; ++i) {
		ap.channels[i].persistant = false;
		alGenSources(1, &(ap.channels[i].al_source));
		alSourcef(ap.channels[i].al_source, AL_PITCH, 1);
		alSourcef(ap.channels[i].al_source, AL_GAIN, 1);
		alSource3f(ap.channels[i].al_source, AL_POSITION, 0, 0, 0);
		alSource3f(ap.channels[i].al_source, AL_VELOCITY, 0, 0, 0);
		alSourcei(ap.channels[i].al_source, AL_LOOPING, AL_FALSE);
	}
	return ap;
}

unsigned int
audioplayer_PlaySound(struct AudioPlayer to_play_from, unsigned int sample, bool persistant, bool looping) {
	return audioplayer_PlaySoundWithVolume(to_play_from, sample, 1.0, persistant, looping);
}

unsigned int
audioplayer_PlaySoundWithVolume(
    struct AudioPlayer to_play_from, unsigned int sample, float volume, bool persistant, bool looping) {
	ALint source_state;
	int i = 0;
	for(; i < MAX_CHANNELS; ++i) {

		alGetSourcei(to_play_from.channels[i].al_source, AL_SOURCE_STATE, &source_state);
		if(!(to_play_from.channels[i].persistant) && (source_state != AL_PLAYING)) {
			to_play_from.channels[i].persistant = persistant;
			alSourcef(to_play_from.channels[i].al_source, AL_GAIN, volume);
			alSourcei(to_play_from.channels[i].al_source, AL_BUFFER, to_play_from.samples[sample].al_buffer);
			if(looping) {
				alSourcei(to_play_from.channels[i].al_source, AL_LOOPING, AL_TRUE);
			} else {
				alSourcei(to_play_from.channels[i].al_source, AL_LOOPING, AL_FALSE);
			}

			alSourcePlay(to_play_from.channels[i].al_source);
			break;
		}
	}
	return i;
}

void
audioplayer_StopSound(struct AudioPlayer to_play_from, unsigned int channel) {
	to_play_from.channels[channel].persistant = false;
	alSourcePause(to_play_from.channels[channel].al_source);
}

unsigned int
audioplayer_LoadSample(struct AudioPlayer* to_load_to, const char* path, unsigned int slot) {
	to_load_to->samples[slot] = audiosample_LoadFile(path);
	return slot;
}