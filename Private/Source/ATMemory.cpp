#include "AT.h"
#include "stdlib.h"
#include "string.h"

int g_MemAllocations = 0;

AT_API void *ATAlloc(size_t size)
{
	++g_MemAllocations;
	return malloc(size);
}

AT_API void ATFree(void* memory)
{
    ATASSERT(memory, "Freeing nullptr memory");
	--g_MemAllocations;
	free(memory);
}

AT_API void ATMemory_VerifyMemory()
{
	ATASSERT(g_MemAllocations == 0, "Memory Leak: ATAlloc call without corresponding ATFree");
}

AT_API void ATMemSet(void* dest, int value, size_t size)
{
	memset(dest, value, size);
}

AT_API void ATMemCopy(void* dest, const void* source, size_t size)
{
	memcpy(dest, source, size);
}

AT_API void ATMemMove(void* dest, const void* source, size_t size)
{
	memmove(dest, source, size);
}
