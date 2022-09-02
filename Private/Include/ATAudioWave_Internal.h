#ifndef AT_AUDIO_WAVE_INTERNAL_H
#define AT_AUDIO_WAVE_INTERNAL_H

struct ATAudioWave;

AT_API void ATAudioWave_Create(ATAudioWave** audioWave, const char* filename, bool looping);

AT_API void ATAudioWave_Destroy(ATAudioWave* audioWave);

AT_API bool ATAudioWave_GetAudioFormat(ATAudioWave* audioWave, ATAudioFormats* format, size_t* frequency);

AT_API bool ATAudioWave_Read(ATAudioWave* audioWave, uint8** data, size_t* readSize);

#endif // AT_AUDIO_WAVE_INTERNAL_H
