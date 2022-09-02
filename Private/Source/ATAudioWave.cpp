#include "AT.h"
#include "ATFile.h"
#include "ATMath.h"
#include "ATAudio_Internal.h"

struct ATAudioWave
{
	ATFile* waveFile;
	uint8* streamBuffer[2];
	uint32 bufferSize[2];
	uint32 sampleRate;
	uint32 pcmSize;
	uint32 pcmPosition;
	uint8 lastFilledBuffer;
	uint8 bitsPerSample;
	uint8 numberChannels;
	uint8 pcmOffset;
	bool looping : 1;
	bool completed : 1;
};

struct WaveFileHeader
{
	char chunkID[4];
	uint32 chunkSize;
	char format[4];
	char subchunk1ID[4];
	uint32 subchunk1Size;
};

struct SubChunk1
{
	uint16 audioFormat;
	uint16 numChannels;
	uint32 sampleRate;
	uint32 byteRate;
	uint16 blockAlign;
	uint16 bitsPerSample;
};

struct SubChunk2
{
	char subchunk2ID[4];
	uint32 subchunk2Size;
};

AT_API void ATAudioWave_Create(ATAudioWave** audioWave, const char* filename, bool looping)
{
	ATAudioWave* wave = (ATAudioWave*)ATAlloc(sizeof(ATAudioWave));
	*audioWave = wave;
	ATMemSet(wave, 0, sizeof(ATAudioWave));

	ATFile_Open(&wave->waveFile, filename);

	char initialReadBuffer[512];
	size_t initialReadSize = sizeof(initialReadBuffer);
	size_t actualReadSize = ATFile_Read(wave->waveFile, initialReadBuffer, initialReadSize);

	WaveFileHeader* header = (WaveFileHeader*)initialReadBuffer;
#ifdef PLATFORM_BIG_ENDIAN
	header->chunkSize = ATEndian_ByteReverse32(header->chunkSize);
	header->subchunk1Size = ATEndian_ByteReverse32(header->subchunk1Size);
#endif
	SubChunk1* sub1 = (SubChunk1*)&initialReadBuffer[sizeof(WaveFileHeader)];
#ifdef PLATFORM_BIG_ENDIAN
	sub1->audioFormat = ATEndian_ByteReverse16(sub1->audioFormat);
	sub1->numChannels = ATEndian_ByteReverse16(sub1->numChannels);
	sub1->sampleRate = ATEndian_ByteReverse32(sub1->sampleRate);
	sub1->byteRate = ATEndian_ByteReverse32(sub1->byteRate);
	sub1->blockAlign = ATEndian_ByteReverse16(sub1->blockAlign);
	sub1->bitsPerSample = ATEndian_ByteReverse16(sub1->bitsPerSample);
#endif
	SubChunk2* sub2 = (SubChunk2*)&((char*)sub1)[header->subchunk1Size];
#ifdef PLATFORM_BIG_ENDIAN
	sub2->subchunk2Size = ATEndian_ByteReverse32(sub2->subchunk2Size);
#endif
	char* pcmData = &((char*)sub2)[sizeof(SubChunk2)];
	size_t pcmDataReadSize = actualReadSize - (pcmData - initialReadBuffer);

	wave->streamBuffer[0] = (uint8*)ATAlloc(AUDIO_BUFFER_SIZE);
	wave->streamBuffer[1] = (uint8*)ATAlloc(AUDIO_BUFFER_SIZE);
	ATMemCopy(wave->streamBuffer[0], pcmData, pcmDataReadSize);
	wave->bufferSize[0] = (uint32)pcmDataReadSize;
	wave->lastFilledBuffer = 0;

	wave->bitsPerSample = (uint8)sub1->bitsPerSample;
	wave->numberChannels = (uint8)sub1->numChannels;
	wave->sampleRate = sub1->sampleRate;
	wave->pcmSize = sub2->subchunk2Size;
	wave->pcmPosition = (uint32)pcmDataReadSize;
	wave->pcmOffset = (uint8)(pcmData - initialReadBuffer);
	wave->looping = looping;

	if (actualReadSize >= sizeof(initialReadBuffer))
	{
		// Fill the buffer
		size_t bufferSize = AUDIO_BUFFER_SIZE - pcmDataReadSize;
		ATFile_Read(wave->waveFile, (char*)&wave->streamBuffer[0][pcmDataReadSize], bufferSize);
		wave->bufferSize[0] += (uint32)bufferSize;
		wave->pcmPosition += bufferSize;

		if (wave->bufferSize[0] < AUDIO_BUFFER_SIZE)
		{
			if (looping)
			{
				ATASSERT(false, "Short looping audio is not currently supported - requires implementation");
			}
            wave->bufferSize[0] = ATMath_Min(wave->bufferSize[0], wave->pcmSize);
			wave->completed = true;
		}
	}
	else if (looping)
	{
		ATASSERT(false, "Short looping audio is not currently supported - requires implementation");
	}
	else
	{
        wave->bufferSize[0] = ATMath_Min(wave->bufferSize[0], wave->pcmSize);
		wave->completed = true;
	}

#ifdef PLATFORM_BIG_ENDIAN
	ATEndian_ByteReverseBuffer(wave->streamBuffer[0], wave->bufferSize[0], sub1->bitsPerSample);
#endif
}

AT_API void ATAudioWave_Destroy(ATAudioWave* audioWave)
{
	ATFile_Close(audioWave->waveFile);
	ATFree(audioWave->streamBuffer[0]);
	ATFree(audioWave->streamBuffer[1]);
	ATFree(audioWave);
}

AT_API bool ATAudioWave_GetAudioFormat(ATAudioWave* audioWave, ATAudioFormats* format, size_t* frequency)
{
	switch (audioWave->numberChannels)
	{
	case 1:
		switch (audioWave->bitsPerSample)
		{
		case 8: *format = ATAF_MONO_8; break;
		case 16: *format = ATAF_MONO_16; break;
		default: ATASSERTRETURN(false, false, "%i bits per sample not supported in wave files", audioWave->bitsPerSample);
		}
		break;
	case 2:
		switch (audioWave->bitsPerSample)
		{
		case 8: *format = ATAF_STEREO_8; break;
		case 16: *format = ATAF_STEREO_16; break;
		default: ATASSERTRETURN(false, false, "%i bits per sample not supported in wave files", audioWave->bitsPerSample);
		}
		break;
	default: ATASSERTRETURN(false, false, "%i channels not supported in wave files", audioWave->numberChannels);
	}

	*frequency = (size_t)audioWave->sampleRate;
	return true;
}

AT_API bool ATAudioWave_Read(ATAudioWave* audioWave, uint8** data, size_t* readSize)
{
	size_t lastFilled = audioWave->lastFilledBuffer;
	*data = audioWave->streamBuffer[lastFilled];
	*readSize = audioWave->bufferSize[lastFilled];

	if (audioWave->completed)
	{
		return true;
	}

	size_t nextFilled = (lastFilled + 1) % 2;
	size_t bufferSize = AUDIO_BUFFER_SIZE;
    uint32 actualReadSize = (uint32)ATFile_Read(audioWave->waveFile, (char*)audioWave->streamBuffer[nextFilled], bufferSize);

	if (actualReadSize < AUDIO_BUFFER_SIZE || audioWave->pcmPosition + actualReadSize > audioWave->pcmSize)
	{
		if (audioWave->looping)
		{
            actualReadSize = ATMath_Min(actualReadSize, audioWave->pcmSize - audioWave->pcmPosition);
			ATFile_Seek(audioWave->waveFile, audioWave->pcmOffset, ATFSM_Start);
			size_t loopBufferSize = AUDIO_BUFFER_SIZE - actualReadSize;
			audioWave->pcmPosition = ATFile_Read(audioWave->waveFile, (char*)&audioWave->streamBuffer[nextFilled][actualReadSize], loopBufferSize);
			actualReadSize += audioWave->pcmPosition;
		}
		else
		{
            actualReadSize = ATMath_Min(actualReadSize, audioWave->pcmSize - audioWave->pcmPosition);
            ATMemSet((char*)&audioWave->streamBuffer[nextFilled][actualReadSize], 0, AUDIO_BUFFER_SIZE - actualReadSize);
			audioWave->completed = true;
		}
	}

	audioWave->pcmPosition += actualReadSize;

	audioWave->bufferSize[nextFilled] = (uint32)actualReadSize;
	audioWave->lastFilledBuffer = (uint8)nextFilled;
	return false;
}
