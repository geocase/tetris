#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sndfile.h>

#include "audio.h"

#define TEST_ERROR(_msg)		\
	error = alGetError();		\
	if (error != AL_NO_ERROR) {	\
		fprintf(stdout, _msg "\n");	\
		return -1;		\
	}

AudioSample_t
audiosample_LoadFile(const char* path) {
    ALCenum error;
    ALuint buffer;
	alGenBuffers(1, &buffer);

	SF_INFO sfinfo;
	SNDFILE* sndfile = sf_open(path, SFM_READ, &sfinfo);
	ALenum format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;

	short* membuf = malloc((sfinfo.frames * sfinfo.channels) * sizeof(short));

	sf_count_t num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if(num_frames < 1) {
		exit(-1);
	}

    ALsizei num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

    AudioSample_t as = {
        .al_buffer = buffer
    };

    return as;
}

struct AudioPlayer audioplayer_Init() {
    struct AudioPlayer ap;
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

unsigned int audioplayer_PlaySound(struct AudioPlayer to_play_from, unsigned int sample, bool persistant, bool looping) {
    ALint source_state;
    int i = 0;
    for(; i < MAX_CHANNELS; ++i) {
        to_play_from.channels[i].persistant = persistant;
        alGetSourcei(to_play_from.channels[i].al_source, AL_SOURCE_STATE, &source_state);
        if(!(to_play_from.channels[i].persistant) && (source_state != AL_PLAYING)) {
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

void audioplayer_StopSound(struct AudioPlayer to_play_from, unsigned int channel) {
    to_play_from.channels[channel].persistant = false;
    alSourcePause(to_play_from.channels[channel].al_source);
}

unsigned int audioplayer_LoadSample(struct AudioPlayer* to_load_to, const char* path, unsigned int slot) {
    to_load_to->samples[slot] = audiosample_LoadFile(path);
    return slot;
}