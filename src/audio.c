#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "audio.h"

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