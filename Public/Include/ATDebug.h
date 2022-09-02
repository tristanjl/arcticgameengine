#ifndef AT_DEBUG_H
#define AT_DEBUG_H

enum ATDebug_LogLevel
{
	ATDLL_Information,
	ATDLL_Warning,
	ATDLL_Error,
	ATDLL_Assert
};

AT_API void ATDebug_LogMessage(ATDebug_LogLevel logLevel, const char* const filename, int lineNumber, const char* outputString, ...);
AT_API bool ATDebug_DisplayAssert(const char* const filename, int lineNumber, const char *outputString, ...);

#if defined(WINDOWS) || defined(WINDOWS_STORE)
#define ATDEBUGBREAK() __debugbreak()
#elif defined (IPHONE)
#if (!TARGET_IPHONE_SIMULATOR)
#define ATDEBUGBREAK() asm("trap")
#else
#define ATDEBUGBREAK() asm("int3")
#endif
#elif defined(ANDROID) || defined(LINUX)
#define ATDEBUGBREAK() __builtin_trap()
#elif defined(EMSCRIPTEN)
#define ATDEBUGBREAK()
#else
#define ATDEBUGBREAK() asm("int3")
#endif

#ifndef ATRETAIL
#define ATIFASSERT (true)
#define ATASSERT(condition, ...) if (!(condition)) { static bool assertOccurred = false; if (!assertOccurred) { assertOccurred = true; if (ATDebug_DisplayAssert(__FILE__, __LINE__, __VA_ARGS__)) { ATDEBUGBREAK(); } } }
#define ATASSERTRETURN(condition, returnValue, message, ...) ATASSERT(condition, message, ##__VA_ARGS__); if (!condition) { return returnValue; }

#define ATLOG(LogLevel, ...) ATDebug_LogMessage(LogLevel, __FILE__, __LINE__, __VA_ARGS__)
#else
#define ATIFASSERT (false)
#define ATASSERT(condition, message)
#define ATASSERTRETURN(condition, message, returnValue) return returnValue
#endif

#endif // AT_DEBUG_H
