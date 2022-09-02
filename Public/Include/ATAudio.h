#ifndef AT_AUDIO_H
#define AT_AUDIO_H

#include "ATTargets.h"
#include "ATTypes.h"

struct ATAudio;
struct ATVector4;

AT_API void ATAudio_Create(ATAudio** audio, const char* filename);

AT_API void ATAudio_Destroy(ATAudio* audio);

AT_API void ATAudio_Play(ATAudio* audio, uint32* reference, bool looping);

AT_API void ATAudio_Stop(uint32 reference);

AT_API void ATAudio_SetVolume(uint32 reference, float volume);

#endif // AT_AUDIO_H
