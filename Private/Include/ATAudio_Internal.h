#ifndef AT_AUDIO_INTERNAL_H
#define AT_AUDIO_INTERNAL_H

static const int MAX_SOURCES = 16; // Consistent source count across all APIs/Platforms
const int NUM_BUFFERS_PER_SOURCE = 2; // Consistent buffer count across all APIs/Platforms
static const size_t AUDIO_BUFFER_SIZE = 8192; // Consistent buffer size across all APIs/Platforms

struct ATAudio;
struct ATVector4;
struct ATAudioSource;
enum ATAudioFormats
{
	ATAF_MONO_8,
	ATAF_MONO_16,
	ATAF_STEREO_8,
	ATAF_STEREO_16
};

enum ATSourceState
{
	ATSS_BufferIndexMask = 0x000000ff,

	ATSS_Active = ATBIT(8),
	ATSS_CompletedRead = ATBIT(9),
	ATSS_Stopping = ATBIT(10),
};

enum ATAudioFileFormats
{
	ATAFF_Wave
};

struct ATAudioSourceData
{
	ATAudioSource* source;
	ATAudio* activeAudio;
	void* activeFile;
	uint32 objectState;
};

struct ATAudio
{
	char* filename;
	int referenceCount;
	uint16 fileType;
};

AT_API bool ATAudio_CreateFile(ATAudio* audio, void** file, bool looping);

AT_API void ATAudio_DestroyFile(void* file, ATAudioFileFormats format);

AT_API bool ATAudio_ReadFile(void* file, ATAudioFileFormats format, uint8** output, size_t* readSize);

AT_API bool ATAudio_GetFileAudioFormat(void* file, ATAudioFileFormats format, ATAudioFormats* audioFormat, size_t* frequency);

AT_API void ATAudio_StopInternal(ATAudioSourceData* data);

AT_API void ATAudio_BufferInternal(ATAudioSourceData* data);

AT_API void ATAudio_PlayPlatformSpecific(ATAudioSourceData* data);
AT_API void ATAudio_StopPlatformSpecific(ATAudioSourceData* data);
AT_API void ATAudio_SetVolumePlatformSpecific(ATAudioSourceData* data, float volume);
AT_API void ATAudio_QueueBufferPlatformSpecific(ATAudioSourceData* data, uint8* fileData, size_t fileDataSize);
AT_API void ATAudio_ClearBuffersPlatformSpecific(ATAudioSourceData* data);

#endif // AT_AUDIO_INTERNAL_H
