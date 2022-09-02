#ifndef AT_TYPES_H
#define AT_TYPES_H

#include <stddef.h>

#define ATBIT(x) (1 << x)
#define ATISPOWEROF2(x) !(x & (x - 1))
//#define ATUNSED(x) ((void)(true ? 0 : ((x), void(), 0)))
#define ATUNSED(x) (void)(x)
#define ATARRAYSIZE(x) (sizeof(x) / sizeof(*x))

#if defined(WINDOWS) || defined(WINDOWS_STORE)
#define ATALIGN_BEGIN(x) __declspec(align(x))
#define ATALIGN_END(x)
#else
#define ATALIGN_BEGIN(x)
#define ATALIGN_END(x) __attribute__((aligned(x)))
#endif

#if defined(WINDOWS) || defined(WINDOWS_STORE)
#define AT_FORCE_INLINE __forceinline
#else
#define AT_FORCE_INLINE inline __attribute__((always_inline))
#endif

typedef char                  int8;
typedef short                 int16;
typedef int                   int32;
typedef long long             int64;

typedef unsigned char         uint8;
typedef unsigned short        uint16;
typedef unsigned int          uint32;
typedef unsigned long long    uint64;

struct ATRect
{
	float x, y, width, height;
};

#endif // AT_TYPES_H
