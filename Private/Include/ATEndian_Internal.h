#ifndef AT_FILE_INTERNAL_H
#define AT_FILE_INTERNAL_H

#include "ATTargets.h"
#include "ATTypes.h"

#include <endian.h>

#if _BYTE_ORDER == _LITTLE_ENDIAN
#define PLATFORM_LITTLE_ENDIAN
#elif _BYTE_ORDER == _BIG_ENDIAN
#define PLATFORM_BIG_ENDIAN
#elif _BYTE_ORDER == _PDP_ENDIAN
#define PLATFORM_PDP_ENDIAN
#endif

AT_API uint16 ATEndian_ByteReverse16(uint16 value)
{
#ifdef ANDROID
	return __swap16md(value);
#endif
}

AT_API uint32 ATEndian_ByteReverse32(uint32 value)
{
#ifdef ANDROID
	return __swap32md(value);
#endif
}

AT_API uint64 ATEndian_ByteReverse64(uint64 value)
{
#ifdef ANDROID
	return __swap64md(value);
#endif
}

AT_API uint16 ATEndian_ByteReverseBuffer(uint8* buffer, size_t bufferSize, size_t elementSize)
{
	switch (elementSize)
	{
	case 8:
		break;
	case 16:
		uint16* pcm16 = (uint16*)buffer;
        for (size_t i = 0; i < bufferSize / 2; ++i)
		{
			__swap16md(*pcm16);
			++pcm16;
		}
		break;
	case 32:
		uint32* pcm32 = (uint32*)buffer;
		for (size_t i = 0; i < bufferSize / 4; ++i)
		{
			__swap32md(*pcm32);
			++pcm32;
		}
		break;
	case 64:
		uint64* pcm64 = (uint64*)buffer;
		for (size_t i = 0; i < bufferSize / 8; ++i)
		{
			__swap64md(*pcm64);
			++pcm64;
		}
		break;
	default:
		ATASSERT(false, "%i Unsupported size for endian flip", bufferSize);
		break;
	}
}

#endif // AT_FILE_INTERNAL_H
