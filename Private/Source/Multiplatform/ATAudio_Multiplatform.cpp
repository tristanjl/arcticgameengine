#include "AT.h"
#include "ATFile.h"
#include "ATAudio.h"
#include "ATVector4.h"
#include "ATAudio_Internal.h"

#if defined(IPHONE) || defined (__MAC_OS__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(LINUX) || defined(EMSCRIPTEN) || defined (WINDOWS) || defined (WINDOWS_GCC)
#include "AL/al.h"
#include "AL/alc.h"
#endif

const int MAX_BUFFERS = MAX_SOURCES * NUM_BUFFERS_PER_SOURCE;

struct ATAudioSource
{
	ALuint source;
	ALuint buffers[NUM_BUFFERS_PER_SOURCE];
	bool buffersQueued[NUM_BUFFERS_PER_SOURCE];
};

struct ATAudioSystem
{
	ATAudioSource audioSources[MAX_SOURCES];
};

ATAudioSystem* g_audioSystem = nullptr;
ALCdevice* g_Device = nullptr;
ALCcontext* g_Context = nullptr;
ALuint g_Sources[MAX_SOURCES];
ALuint g_Buffers[MAX_BUFFERS];

AT_API void ATAudio_InitPlatformSpecific(ATAudioSourceData* data)
{
	g_audioSystem = (ATAudioSystem*)ATAlloc(sizeof(ATAudioSystem));
	ATMemSet(g_audioSystem, 0, sizeof(ATAudioSystem));

    g_Device = alcOpenDevice(nullptr);
	ALenum error = alGetError();
	if (!g_Device)
	{
		ATLOG(ATDLL_Error, "%i error OpenAL", error);
		return;
	}

	g_Context = alcCreateContext(g_Device, 0);
	if (!g_Context)
	{
		alcCloseDevice(g_Device);
		return;
	}
	alcMakeContextCurrent(g_Context);

	alGenSources(MAX_SOURCES, g_Sources);
	ATASSERT(alGetError() == AL_NO_ERROR, "Error generating Audio Sources");
	alGenBuffers(MAX_BUFFERS, g_Buffers);
	ATASSERT(alGetError() == AL_NO_ERROR, "Error generating Audio Buffers");
	for (int i = 0; i < MAX_SOURCES; ++i)
	{
		ATAudioSource* source = &g_audioSystem->audioSources[i];
		data[i].source = source;
		source->source = g_Sources[i];
		for (int j = 0; j < NUM_BUFFERS_PER_SOURCE; ++j)
		{
			source->buffers[j] = g_Buffers[i * NUM_BUFFERS_PER_SOURCE + j];
		}
	}
}

static ALenum ATAudio_GetOpenALFormat(ATAudioFormats format)
{
	switch (format)
	{
	case ATAF_MONO_8:     return AL_FORMAT_MONO8;
	case ATAF_MONO_16:    return AL_FORMAT_MONO16;
	case ATAF_STEREO_8:   return AL_FORMAT_STEREO8;
	case ATAF_STEREO_16:  return AL_FORMAT_STEREO16;
	}
	return 0;
}

AT_API void ATAudio_PlayPlatformSpecific(ATAudioSourceData* data)
{
	alSourcePlay(data->source->source);
}

AT_API void ATAudio_StopPlatformSpecific(ATAudioSourceData* data)
{
	alSourceStop(data->source->source);
}

AT_API void ATAudio_SetVolumePlatformSpecific(ATAudioSourceData* data, float volume)
{
	alSourcef(data->source->source, AL_GAIN, volume);
}

AT_API void ATAudio_QueueBufferPlatformSpecific(ATAudioSourceData* data, uint8* fileData, size_t fileDataSize)
{
	ATAudio* audio = data->activeAudio;
	ATAudioSource* source = data->source;

	size_t bufferIndex = data->objectState & ATSS_BufferIndexMask;
	ATAudioFormats format;
	size_t frequency;

	ATAudio_GetFileAudioFormat(data->activeFile, (ATAudioFileFormats)audio->fileType, &format, &frequency);
	if (source->buffersQueued[bufferIndex])
	{
		alSourceUnqueueBuffers(source->source, 1, &source->buffers[bufferIndex]);
	}
	else
	{
		source->buffersQueued[bufferIndex] = true;
	}
	alBufferData(source->buffers[bufferIndex], ATAudio_GetOpenALFormat(format), fileData, (ALsizei)fileDataSize, (ALsizei)frequency);
	alSourceQueueBuffers(source->source, 1, &source->buffers[bufferIndex]);
	data->objectState = (bufferIndex + 1) % NUM_BUFFERS_PER_SOURCE + (data->objectState & ~ATSS_BufferIndexMask);
}

AT_API void ATAudio_ClearBuffersPlatformSpecific(ATAudioSourceData* data)
{
	ATAudioSource* source = data->source;

	for (int i = 0; i < NUM_BUFFERS_PER_SOURCE; ++i)
	{
		if (source->buffersQueued[i])
		{
			alSourceUnqueueBuffers(source->source, 1, &source->buffers[i]);
			source->buffersQueued[i] = false;
		}
	}
}

AT_API void ATAudio_DeinitPlatformSpecific()
{
	alDeleteBuffers(MAX_BUFFERS, g_Buffers);
	alDeleteSources(MAX_SOURCES, g_Sources);
	alcMakeContextCurrent(0);
	alcDestroyContext(g_Context);
	alcCloseDevice(g_Device);

	ATFree(g_audioSystem);
    g_audioSystem = nullptr;
}

AT_API void ATAudio_UpdatePlatformSpecific(ATAudioSourceData* data)
{
	for (int i = 0; i < MAX_SOURCES; ++i)
	{
		ATAudioSourceData* sourceData = &data[i];
		ATAudioSource* source = &g_audioSystem->audioSources[i];
		if (!(sourceData->objectState & (ATSS_Active)) || sourceData->objectState & (ATSS_CompletedRead | ATSS_Stopping))
		{
			continue;
		}

		ALint processed;
		alGetSourcei(source->source, AL_BUFFERS_PROCESSED, &processed);
		for (int j = 0; j < processed; ++j)
		{
			ATAudio_BufferInternal(sourceData);
		}
		ALint state;
		alGetSourcei(source->source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING)
		{
			ATAudio_PlayPlatformSpecific(sourceData);
		}
	}
}
