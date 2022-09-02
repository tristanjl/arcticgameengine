#include "AT.h"
#include "ATAudio_Internal.h"

#include <math.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

const size_t MAX_PLAYERS = 16;

struct ATAudioSource
{
	SLObjectItf slAudioPlayerObject;
};

struct ATAudioSystem
{
	SLEngineItf slEngine;
	SLObjectItf slEngineObject;
	SLObjectItf slOutputMixObject;
	ATAudioSource audioPlayers[MAX_PLAYERS];
	bool initialised;
	uint32 allocatedAudioObjects;
};

ATAudioSystem* g_audioSystem;

static void ATAudio_PlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context)
{
	ATAudioSourceData* data = (ATAudioSourceData*)context;

	if (data->objectState & ATSS_CompletedRead)
	{
		ATAudio_StopInternal(data);
	}
	else
	{
		ATAudio_BufferInternal(data);
	}
}

AT_API void ATAudio_InitPlatformSpecific(ATAudioSourceData* data)
{
	g_audioSystem = (ATAudioSystem *)ATAlloc(sizeof(ATAudioSystem));
	ATMemSet(g_audioSystem, 0, sizeof(ATAudioSystem));

	SLresult result = slCreateEngine(&g_audioSystem->slEngineObject, 0, NULL, 0, NULL, NULL);

	if (result != SL_RESULT_SUCCESS)
	{
		return;
	}

	result = (*g_audioSystem->slEngineObject)->Realize(g_audioSystem->slEngineObject, SL_BOOLEAN_FALSE);
	if (result != SL_RESULT_SUCCESS)
	{
		return;
	}

	result = (*g_audioSystem->slEngineObject)->GetInterface(g_audioSystem->slEngineObject, SL_IID_ENGINE, &g_audioSystem->slEngine);
	if (result != SL_RESULT_SUCCESS)
	{
		return;
	}

	const SLInterfaceID ids[] = {};
	const SLboolean req[] = {};

	result = (*g_audioSystem->slEngine)->CreateOutputMix(g_audioSystem->slEngine, &g_audioSystem->slOutputMixObject, 0, ids, req);
	
	if (result != SL_RESULT_SUCCESS)
	{
		return;
	}

	result = (*g_audioSystem->slOutputMixObject)->Realize(g_audioSystem->slOutputMixObject, SL_BOOLEAN_FALSE);

	if (result != SL_RESULT_SUCCESS)
	{
		return;
	}

	for (size_t i = 0; i < MAX_PLAYERS; ++i)
	{
		SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
		SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_44_1,
			SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16, SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
		SLDataSource audioSrc = {&loc_bufq, &format_pcm};

		SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, g_audioSystem->slOutputMixObject};
		SLDataSink audioSnk = {&loc_outmix, NULL};

		const SLInterfaceID playerIds[] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
		const SLboolean playerReq[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

		SLObjectItf& audioPlayer = g_audioSystem->audioPlayers[i].slAudioPlayerObject;
		SLresult result = (*g_audioSystem->slEngine)->CreateAudioPlayer(g_audioSystem->slEngine, &audioPlayer, &audioSrc, &audioSnk, 3, playerIds, playerReq);

		if (result != SL_RESULT_SUCCESS)
		{
			return;
		}

		result = (*audioPlayer)->Realize(audioPlayer, SL_BOOLEAN_FALSE);

		if (result != SL_RESULT_SUCCESS)
		{
			return;
		}

		SLAndroidSimpleBufferQueueItf playerBufferQueue;
		result = (*audioPlayer)->GetInterface(audioPlayer, SL_IID_BUFFERQUEUE, &playerBufferQueue);

		if (result != SL_RESULT_SUCCESS)
		{
			return;
		}

		result = (*playerBufferQueue)->RegisterCallback(playerBufferQueue, ATAudio_PlayerCallback, &data[i]);

		if (result != SL_RESULT_SUCCESS)
		{
			return;
		}

		data[i].source = &g_audioSystem->audioPlayers[i];
		g_audioSystem->allocatedAudioObjects = i;
	}
}

AT_API void ATAudio_PlayPlatformSpecific(ATAudioSourceData* data)
{
	SLPlayItf slPlayer;
	(*data->source->slAudioPlayerObject)->GetInterface(data->source->slAudioPlayerObject, SL_IID_PLAY, &slPlayer);
	(*slPlayer)->SetPlayState(slPlayer, SL_PLAYSTATE_PLAYING);
}

AT_API void ATAudio_StopPlatformSpecific(ATAudioSourceData* data)
{
	SLPlayItf slPlayer;
	(*data->source->slAudioPlayerObject)->GetInterface(data->source->slAudioPlayerObject, SL_IID_PLAY, &slPlayer);
	(*slPlayer)->SetPlayState(slPlayer, SL_PLAYSTATE_STOPPED);
}

float gain_to_attenuation(float gain)
{
	return gain < 0.01F ? -96.0F : 20 * log10(gain);
}

AT_API void ATAudio_SetVolumePlatformSpecific(ATAudioSourceData* data, float volume)
{
	SLVolumeItf slVolume;
	(*data->source->slAudioPlayerObject)->GetInterface(data->source->slAudioPlayerObject, SL_IID_VOLUME, &slVolume);
	(*slVolume)->SetVolumeLevel(slVolume, gain_to_attenuation(volume) * 100.0f);
}

AT_API void ATAudio_ClearBuffersPlatformSpecific(ATAudioSourceData* data)
{
	SLAndroidSimpleBufferQueueItf playerBufferQueue;
	(*data->source->slAudioPlayerObject)->GetInterface(data->source->slAudioPlayerObject, SL_IID_BUFFERQUEUE, &playerBufferQueue);
	(*playerBufferQueue)->Clear(playerBufferQueue);
}

AT_API void ATAudio_QueueBufferPlatformSpecific(ATAudioSourceData* data, uint8* fileData, size_t fileDataSize)
{
	SLAndroidSimpleBufferQueueItf playerBufferQueue;
	(*data->source->slAudioPlayerObject)->GetInterface(data->source->slAudioPlayerObject, SL_IID_BUFFERQUEUE, &playerBufferQueue);
	(*playerBufferQueue)->Enqueue(playerBufferQueue, fileData, fileDataSize);
}

AT_API void ATAudio_DeinitPlatformSpecific()
{
	for (size_t i = 0; i < MAX_PLAYERS; ++i)
	{
		SLObjectItf& audioPlayer = g_audioSystem->audioPlayers[i].slAudioPlayerObject;
		if (audioPlayer)
		{
			(*audioPlayer)->Destroy(audioPlayer);
		}
	}

	if (g_audioSystem->slOutputMixObject != NULL)
	{
		(*g_audioSystem->slOutputMixObject)->Destroy(g_audioSystem->slOutputMixObject);
	}

	if (g_audioSystem->slEngineObject != NULL)
	{
		(*g_audioSystem->slEngineObject)->Destroy(g_audioSystem->slEngineObject);
	}

	ATFree(g_audioSystem);
}

AT_API void ATAudio_UpdatePlatformSpecific()
{
}
