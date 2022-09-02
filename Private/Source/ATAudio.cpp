#include "AT.h"
#include "ATAudio.h"
#include "ATResource.h"
#include "ATHashTable.h"
#include "ATAudio_Internal.h"
#include "ATAudioWave_Internal.h"

typedef ATHashTable<ATAudio, 32> AudioHashTable;
AudioHashTable* g_AudioTable;

ATAudioSourceData* g_SourceData;

AT_API void ATAudio_Init()
{
	g_AudioTable = ATNew(AudioHashTable);
	g_SourceData = (ATAudioSourceData*)ATAlloc(sizeof(ATAudioSourceData) * MAX_SOURCES);
	ATMemSet(g_SourceData, 0, sizeof(ATAudioSourceData) * MAX_SOURCES);

	void ATAudio_InitPlatformSpecific(ATAudioSourceData*); ATAudio_InitPlatformSpecific(g_SourceData);
}

AT_API void ATAudio_Deinit()
{
	for (int i = 0; i < MAX_SOURCES; ++i)
	{
		if (g_SourceData[i].objectState & ATSS_Active)
		{
			g_SourceData[i].objectState &= ~ATSS_CompletedRead; // Clear flag to stop audio immediately
			ATAudio_StopInternal(&g_SourceData[i]);
		}
	}

	void ATAudio_DeinitPlatformSpecific(); ATAudio_DeinitPlatformSpecific();

	ATFree(g_SourceData);
	ATDelete(AudioHashTable, g_AudioTable);
}

AT_API void ATAudio_Update()
{
	void ATAudio_UpdatePlatformSpecific(ATAudioSourceData*); ATAudio_UpdatePlatformSpecific(g_SourceData);
}

AT_API void ATAudio_Create(ATAudio** audio, const char* filename)
{
	ATAudio* audioFile = g_AudioTable->Get(filename);
	if (!audioFile)
	{
		audioFile = (ATAudio *)ATAlloc(sizeof(ATAudio));

		size_t filenameLength = ATString_Length(filename);
		++filenameLength;
		audioFile->filename = (char *)ATAlloc(filenameLength);
		ATString_Copy(audioFile->filename, filenameLength, filename);
		audioFile->referenceCount = 0;
		const char* extension = ATString_FindCharReverse(filename, '.');
		ATASSERT(extension, "Invalid filename: %s - No file extension", filename);
		if (!ATString_Compare(extension, ".wav"))
		{
			audioFile->fileType = ATAFF_Wave;
		}
		else
		{
			ATASSERT(false, "Unsupported audio file type %s", extension);
		}
		g_AudioTable->Add(filename, audioFile);
	}
	++audioFile->referenceCount;
	*audio = audioFile;
}

AT_API void ATAudio_Play(ATAudio* audio, uint32* reference, bool looping)
{
	size_t i;
	for (i = 0; i < MAX_SOURCES; ++i)
	{
		if (!(g_SourceData[i].objectState & ATSS_Active))
		{
			break;
		}
	}
	if (i >= MAX_SOURCES)
	{
		return; // No player available to play the audio - ignore the play call
	}

	if (!ATAudio_CreateFile(audio, &g_SourceData[i].activeFile, looping))
	{
		return;
	}

	++audio->referenceCount;
	g_SourceData[i].activeAudio = audio;

	*reference = (uint32)i;

	for (int bufferIndex = 0; bufferIndex < NUM_BUFFERS_PER_SOURCE; ++bufferIndex)
	{
		ATAudio_BufferInternal(&g_SourceData[i]);
	}

	ATAudio_PlayPlatformSpecific(&g_SourceData[i]);
	g_SourceData[i].objectState |= ATSS_Active;
}

AT_API void ATAudio_Stop(uint32 reference)
{
	if (reference > MAX_SOURCES)
	{
		return;
	}

	ATAudio_StopInternal(&g_SourceData[reference]);
}

AT_API void ATAudio_SetVolume(uint32 reference, float volume)
{
	ATAudio_SetVolumePlatformSpecific(&g_SourceData[reference], volume);
}

AT_API void ATAudio_Destroy(ATAudio* audio)
{
	--audio->referenceCount;

	if (audio->referenceCount == 0)
	{
		g_AudioTable->Remove(audio->filename);
		ATFree(audio->filename);
		ATFree(audio);
	}
}

AT_API bool ATAudio_CreateFile(ATAudio* audio, void** file, bool looping)
{
	char filePath[512];
	ATResource_GetPath(filePath, ATARRAYSIZE(filePath));
	ATString_Concatenate(filePath, ATARRAYSIZE(filePath), audio->filename);

	switch (audio->fileType)
	{
	case ATAFF_Wave:
		{
			ATAudioWave_Create((ATAudioWave**)file, filePath, looping);
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

AT_API void ATAudio_DestroyFile(void* file, ATAudioFileFormats format)
{
	switch (format)
	{
	case ATAFF_Wave:
		{
			ATAudioWave_Destroy((ATAudioWave*)file);
		}
		break;
	default:
		break;
	}
}

AT_API bool ATAudio_ReadFile(void* file, ATAudioFileFormats format, uint8** output, size_t* readSize)
{
	switch (format)
	{
	case ATAFF_Wave:
		{
			return ATAudioWave_Read((ATAudioWave*)file, output, readSize);
		}
		break;
	default:
		break;
	}
	return true;
}

AT_API bool ATAudio_GetFileAudioFormat(void* file, ATAudioFileFormats format, ATAudioFormats* audioFormat, size_t* frequency)
{
	switch (format)
	{
	case ATAFF_Wave:
		{
			return ATAudioWave_GetAudioFormat((ATAudioWave*)file, audioFormat, frequency);
		}
		break;
	default:
		break;
	}
	return true;
}

AT_API void ATAudio_StopInternal(ATAudioSourceData* data)
{
	if (data->objectState & ATSS_Stopping)
	{
		return;
	}

	data->objectState |= ATSS_Stopping;

	ATAudio_StopPlatformSpecific(data);

	ATAudio_ClearBuffersPlatformSpecific(data);

	ATAudio_DestroyFile(data->activeFile, (ATAudioFileFormats)data->activeAudio->fileType);
	ATAudio_Destroy(data->activeAudio);
	data->objectState = 0;
    data->activeAudio = nullptr;
}

AT_API void ATAudio_BufferInternal(ATAudioSourceData* data)
{
	ATAudio* audio = data->activeAudio;

	uint8* buffer;
	size_t readSize;

	bool completed = ATAudio_ReadFile(data->activeFile, (ATAudioFileFormats)audio->fileType, &buffer, &readSize);

	if (completed)
	{
		if (readSize > 0)
		{
			data->objectState |= (uint32)ATSS_CompletedRead;
		}
		else
		{
			ATAudio_StopInternal(data);
			return;
		}
	}

	ATAudio_QueueBufferPlatformSpecific(data, buffer, readSize);
}
