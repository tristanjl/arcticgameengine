#ifndef AT_TARGETS_H
#define AT_TARGETS_H

#if defined (EMSCRIPTEN) && defined(ATEDITOR)
#include "Emscripten/emscripten.h"
#define AT_API extern "C" EMSCRIPTEN_KEEPALIVE
#elif __cplusplus
#define AT_API extern "C"
#else
#define AT_API
#endif

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4668) // is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning(disable : 4710) // function not inlined
#pragma warning(disable : 4820) // bytes padding added after data member
#ifdef WINAPI_FAMILY
#include <winapifamily.h>

#if (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#define WINDOWS_STORE
#pragma warning(disable : 4061) // enumerator in switch of enum is not explicitly handled by case label
#pragma warning(disable : 4062) // enumerator in switch of enum is not handled
#pragma warning(disable : 4350) // behaviour change
#pragma warning(disable : 4365) // signed/unsigned mismatch
#pragma warning(disable : 4571) // catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
#pragma warning(disable : 4625) // copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable : 4626) // assignment operator could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable : 4917) // a GUID can only be associated with a class, interface or namespace
#pragma warning(disable : 4946) // reinterpret_cast used between related classes
#endif

#else
#define WINDOWS
#pragma warning(disable : 4127) // unreferenced inline function has been removed
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#pragma warning(disable : 4711) // function selected for automatic inline expansion
#endif

#endif

#ifdef __CYGWIN__
#define WINDOWS_GCC
#endif

#ifdef __APPLE__
#include "TargetConditionals.h"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#define IPHONE
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#endif
#ifdef TARGET_OS_MAC
#define MACINTOSH
#endif
#endif

#ifdef __linux__
#define LINUX
#endif

#ifdef __ANDROID__
#include "stdarg.h"
#ifndef ANDROID
#define ANDROID
#endif
#endif

#endif // AT_TARGETS_H
