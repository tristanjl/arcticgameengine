#ifndef AT_INPUT_ANDROID_INTERNAL_H
#define AT_INPUT_ANDROID_INTERNAL_H

//#define ENABLE_MOGA_SUPPORT_ANDROID
#define ENABLE_OUYA_SUPPORT_ANDROID

#ifdef ENABLE_MOGA_SUPPORT_ANDROID
AT_API void ATInput_InitMOGA();
AT_API void ATInput_DeinitMOGA();
AT_API void ATInput_UpdateMOGA();
#endif

#ifdef ENABLE_OUYA_SUPPORT_ANDROID
AT_API void ATInput_InitOUYA();
AT_API void ATInput_DeinitOUYA();
AT_API void ATInput_UpdateOUYA();
AT_API size_t ATInput_GetGamepadIndexByDeviceIdOUYA(int deviceId);
#endif

#endif // AT_INPUT_ANDROID_INTERNAL_H
