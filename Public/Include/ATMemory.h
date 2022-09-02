#ifndef AT_MEMORY_H
#define AT_MEMORY_H

#include <new>

#define ATNew(T, ...) (new (ATAlloc(sizeof(T))) T(__VA_ARGS__))
#define ATArrayNew(T, count) (new (ATAlloc(sizeof(T[count]))) T[count])
#define ATDelete(T, memory) ((T*)memory)->~T(); ATFree(memory)
#define ATArrayDelete(T, count, memory) ( for (size_t i = 0; i < count; ++i) { ((T**)memory)[i]->~T(); } ATFree(memory)

AT_API void *ATAlloc(size_t size);

AT_API void ATFree(void* memory);

AT_API void ATMemSet(void* dest, int value, size_t size);

AT_API void ATMemCopy(void* dest, const void* source, size_t size);

AT_API void ATMemMove(void* dest, const void* source, size_t size);

#endif // AT_MEMORY_H
