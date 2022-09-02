#include "AT.h"
#include "ATInput_Internal.h"
#include "Android/ATInput_Android_Internal.h"
#include "Android/ATSystem_Android_Internal.h"

#ifdef ENABLE_MOGA_SUPPORT_ANDROID
#include <jni.h>

static JNIEnv* s_JNIEnv = NULL;
static jobject s_Controller;

static jclass mClass;
static jmethodID mMethodGetInstance;
static jmethodID mMethodInit;
static jmethodID mMethodExit;
static jmethodID mMethodGetAxisValue;
static jmethodID mMethodGetInfo;
static jmethodID mMethodGetKeyCode;
static jmethodID mMethodGetState;
static jmethodID mMethodOnPause;
static jmethodID mMethodOnResume;
static jmethodID mMethodSetListener;

class Controller
{
public:
	static const jint ACTION_DOWN = 0;
	static const jint ACTION_UP = 1;
	static const jint ACTION_FALSE = 0;
	static const jint ACTION_TRUE = 1;
	static const jint ACTION_DISCONNECTED = 0;
	static const jint ACTION_CONNECTED = 1;
	static const jint ACTION_CONNECTING = 2;
	static const jint ACTION_VERSION_MOGA = 0;
	static const jint ACTION_VERSION_MOGAPRO = 1;

	static const jint KEYCODE_DPAD_UP = 19;
	static const jint KEYCODE_DPAD_DOWN = 20;
	static const jint KEYCODE_DPAD_LEFT = 21;
	static const jint KEYCODE_DPAD_RIGHT = 22;
	static const jint KEYCODE_BUTTON_A = 96;
	static const jint KEYCODE_BUTTON_B = 97;
#ifdef BUILD_LEGACY_API
	// SDK 1.2.7b and earlier
	static const jint KEYCODE_BUTTON_X = 98;
	static const jint KEYCODE_BUTTON_Y = 99;
#else
	// SDK 1.2.7c and later
	static const jint KEYCODE_BUTTON_X = 99;
	static const jint KEYCODE_BUTTON_Y = 100;
#endif
	static const jint KEYCODE_BUTTON_L1 = 102;
	static const jint KEYCODE_BUTTON_R1 = 103;
	static const jint KEYCODE_BUTTON_L2 = 104;
	static const jint KEYCODE_BUTTON_R2 = 105;
	static const jint KEYCODE_BUTTON_THUMBL = 106;
	static const jint KEYCODE_BUTTON_THUMBR = 107;
	static const jint KEYCODE_BUTTON_START = 108;
	static const jint KEYCODE_BUTTON_SELECT = 109;

	static const jint INFO_KNOWN_DEVICE_COUNT = 1;
	static const jint INFO_ACTIVE_DEVICE_COUNT = 2;

	static const jint AXIS_X = 0;
	static const jint AXIS_Y = 1;
	static const jint AXIS_Z = 11;
	static const jint AXIS_RZ = 14;
	static const jint AXIS_LTRIGGER = 17;
	static const jint AXIS_RTRIGGER = 18;

	static const jint STATE_CONNECTION = 1;
	static const jint STATE_POWER_LOW = 2;
	static const jint STATE_SUPPORTED_VERSION = 3;	// Controller Version
	static const jint STATE_CURRENT_PRODUCT_VERSION = 4;	// Service Controller Version

public:
	Controller(jobject controller) : mController(controller)
	{
	}

	static void jniOnLoad(JNIEnv * env, jobject activity)
	{
		// get package-aware ClassLoader
		const jclass nativeActivityCls = env->FindClass("android/app/NativeActivity");
		const jmethodID getClassLoaderMth = env->GetMethodID(nativeActivityCls, "getClassLoader", "()Ljava/lang/ClassLoader;");
		const jobject classLoader = env->CallObjectMethod(activity, getClassLoaderMth);

		// load class
		const jstring classString = env->NewStringUTF("com/bda/controller/Controller");
		const jclass classLoaderCls = env->FindClass("java/lang/ClassLoader");
		const jmethodID loadClassMth = env->GetMethodID(classLoaderCls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;"); 
		const jclass cls = static_cast<jclass>(env->CallObjectMethod(classLoader, loadClassMth, classString));
		env->DeleteLocalRef(classString);

		mClass = static_cast<jclass>(env->NewWeakGlobalRef(cls));

		// cache method ids
		mMethodGetInstance = env->GetStaticMethodID(cls, "getInstance", "(Landroid/content/Context;)Lcom/bda/controller/Controller;");
		mMethodInit = env->GetMethodID(cls, "init", "()Z");
		mMethodExit = env->GetMethodID(cls, "exit", "()V");
		mMethodGetAxisValue = env->GetMethodID(cls, "getAxisValue", "(I)F");
		mMethodGetInfo = env->GetMethodID(cls, "getInfo", "(I)I");
		mMethodGetKeyCode = env->GetMethodID(cls, "getKeyCode", "(I)I");
		mMethodGetState = env->GetMethodID(cls, "getState", "(I)I");
		mMethodOnPause = env->GetMethodID(cls, "onPause", "()V");
		mMethodOnResume = env->GetMethodID(cls, "onResume", "()V");
		mMethodSetListener = env->GetMethodID(cls, "setListener", "(Lcom/bda/controller/ControllerListener;Landroid/os/Handler;)V");
	}

	static void jniOnUnload(JNIEnv * env)
	{
		env->DeleteWeakGlobalRef(mClass);
	}

	static jobject getInstance(JNIEnv * env, jobject context)
	{
		return env->CallStaticObjectMethod(mClass, mMethodGetInstance, context);
	}

	jboolean init(JNIEnv * env)
	{
		return env->CallBooleanMethod(mController, mMethodInit);
	}

	void exit(JNIEnv * env)
	{
		env->CallVoidMethod(mController, mMethodExit);
	}

	jfloat getAxisValue(JNIEnv * env, jint axis)
	{
		return env->CallFloatMethod(mController, mMethodGetAxisValue, axis);
	}

	jint getInfo(JNIEnv * env, jint keyCode)
	{
		return env->CallIntMethod(mController, mMethodGetInfo, keyCode);
	}

	jint getKeyCode(JNIEnv * env, jint keyCode)
	{
		return env->CallIntMethod(mController, mMethodGetKeyCode, keyCode);
	}

	jint getState(JNIEnv * env, jint state)
	{
		return env->CallIntMethod(mController, mMethodGetState, state);
	}

	void onPause(JNIEnv * env)
	{
		env->CallVoidMethod(mController, mMethodOnPause);
	}

	void onResume(JNIEnv * env)
	{
		env->CallVoidMethod(mController, mMethodOnResume);
	}

	void setListener(JNIEnv * env, jobject listener, jobject handler)
	{
		env->CallVoidMethod(mController, mMethodSetListener, listener, handler);
	}

private:
	const jobject mController;
};

AT_API void ATInput_InitMOGA()
{
	android_app* app = ATSystem_Android_GetApp();
	app->activity->vm->AttachCurrentThread(&s_JNIEnv, NULL);

	Controller::jniOnLoad(s_JNIEnv, app->activity->clazz);

	const jobject jController = Controller::getInstance(s_JNIEnv, app->activity->clazz);
	s_Controller = s_JNIEnv->NewGlobalRef(jController);

	Controller controller(s_Controller);
	controller.init(s_JNIEnv);
}

AT_API void ATInput_DeinitMOGA()
{
	Controller controller(s_Controller);
	controller.exit(s_JNIEnv);

	s_JNIEnv->DeleteGlobalRef(s_Controller);
	s_Controller = NULL;

	Controller::jniOnUnload(s_JNIEnv);

	ATSystem_Android_GetApp()->activity->vm->DetachCurrentThread();
}

AT_API void ATInput_UpdateMOGA()
{
	Controller controller(s_Controller);

	GamepadData* const data = &s_GamepadData[0];
	ATMemCopy(data->previousInputStates, data->inputStates, sizeof(data->previousInputStates));
	data->active = true;

	bool usingPocketController = controller.getState(s_JNIEnv, Controller::STATE_CURRENT_PRODUCT_VERSION) == Controller::ACTION_VERSION_MOGA;

	data->inputStates[ATGIM_XAxis1] = controller.getAxisValue(s_JNIEnv, Controller::AXIS_X);
	data->inputStates[ATGIM_YAxis1] = -controller.getAxisValue(s_JNIEnv, Controller::AXIS_Y);
	data->inputStates[ATGIM_XAxis2] = controller.getAxisValue(s_JNIEnv, Controller::AXIS_Z);
	data->inputStates[ATGIM_YAxis2] = -controller.getAxisValue(s_JNIEnv, Controller::AXIS_RZ);

	if (!usingPocketController)
	{
		data->inputStates[ATGIM_DPadUp] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_DPAD_UP)  == Controller::ACTION_DOWN;
		data->inputStates[ATGIM_DPadDown] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_DPAD_DOWN)  == Controller::ACTION_DOWN;
		data->inputStates[ATGIM_DPadLeft] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_DPAD_LEFT)  == Controller::ACTION_DOWN;
		data->inputStates[ATGIM_DPadRight] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_DPAD_RIGHT)  == Controller::ACTION_DOWN;
	}
	data->inputStates[ATGIM_Button1] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_A)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_Button2] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_B)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_Button3] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_X)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_Button4] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_Y)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_Menu1] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_START)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_Menu2] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_SELECT)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_L1] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_L1)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_R1] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_R1)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_LeftAxisButton] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_THUMBL)  == Controller::ACTION_DOWN;
	data->inputStates[ATGIM_RightAxisButton] = controller.getKeyCode(s_JNIEnv, Controller::KEYCODE_BUTTON_THUMBR)  == Controller::ACTION_DOWN;

	data->inputStates[ATGIM_L2] = controller.getAxisValue(s_JNIEnv, Controller::AXIS_LTRIGGER);
	data->inputStates[ATGIM_R2] = controller.getAxisValue(s_JNIEnv, Controller::AXIS_RTRIGGER);
}

#endif
