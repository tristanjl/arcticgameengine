#include "AT.h"
#include "ATMath.h"
#include "ATInput_Internal.h"
#include "Android/ATInput_Android_Internal.h"
#include "Android/ATSystem_Android_Internal.h"

#ifdef ENABLE_OUYA_SUPPORT_ANDROID
#include <jni.h>
#include <android/log.h>

static const size_t OUYA_DEVICE_COUNT = 4;

static JNIEnv* s_JNIEnv = NULL;

static jclass mClass;
static jmethodID mGetPlayerNumByDeviceId;

AT_API void ClearJNIExceptions()
{
	if (s_JNIEnv && s_JNIEnv->ExceptionOccurred())
	{
		s_JNIEnv->ExceptionDescribe();
		s_JNIEnv->ExceptionClear();
	}
}

static void ATInput_OuyaJniOnLoad(JNIEnv * env, jobject activity)
{
	// get package-aware ClassLoader
	const jclass nativeActivityCls = env->FindClass("android/app/NativeActivity");
	const jmethodID getClassLoaderMth = env->GetMethodID(nativeActivityCls, "getClassLoader", "()Ljava/lang/ClassLoader;");
	const jobject classLoader = env->CallObjectMethod(activity, getClassLoaderMth);

	// load class
	const jstring classString = env->NewStringUTF("tv/ouya/console/api/OuyaController");
	const jclass classLoaderCls = env->FindClass("java/lang/ClassLoader");
	const jmethodID loadClassMth = env->GetMethodID(classLoaderCls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	const jclass cls = static_cast<jclass>(env->CallObjectMethod(classLoader, loadClassMth, classString));
	env->DeleteLocalRef(classString);

	mClass = static_cast<jclass>(env->NewWeakGlobalRef(cls));

	// cache method ids
	mGetPlayerNumByDeviceId = env->GetStaticMethodID(cls, "getPlayerNumByDeviceId", "(I)I");
}

static void ATInput_OuyaJniOnUnload(JNIEnv * env)
{
	env->DeleteWeakGlobalRef(mClass);
}

AT_API size_t ATInput_GetGamepadIndexByDeviceIdOUYA(int deviceId)
{
	if (s_JNIEnv)
	{
		return s_JNIEnv->CallStaticIntMethod(mClass, mGetPlayerNumByDeviceId, deviceId);
		ClearJNIExceptions();
	}
	return 0;
}

AT_API void ATInput_InitOUYA()
{
	android_app* app = ATSystem_Android_GetApp();
	app->activity->vm->AttachCurrentThread(&s_JNIEnv, NULL);

	ATInput_OuyaJniOnLoad(s_JNIEnv, app->activity->clazz);

	for (size_t i = 0; i < OUYA_DEVICE_COUNT; ++i)
	{
		s_GamepadData[i].active = true;
	}
}

AT_API void ATInput_DeinitOUYA()
{
	ATInput_OuyaJniOnUnload(s_JNIEnv);

	s_JNIEnv = NULL;

	ATSystem_Android_GetApp()->activity->vm->DetachCurrentThread();
}

AT_API void ATInput_UpdateOUYA()
{
	for (size_t i = 0; i < OUYA_DEVICE_COUNT; ++i)
	{
		GamepadData* const data = &s_GamepadData[i];

		ATMemCopy(data->previousInputStates, data->inputStates, sizeof(data->previousInputStates));
	}
}

#endif // ENABLE_OUYA_SUPPORT_ANDROID
