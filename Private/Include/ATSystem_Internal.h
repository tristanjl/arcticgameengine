#ifndef AT_SYSTEM_H
#define AT_SYSTEM_H

AT_API void ATSystem_InitialiseSystems();

AT_API void ATSystem_DeinitialiseSystems();

AT_API bool ATSystem_Update();

AT_API void ATSystem_Suspend();

AT_API void ATSystem_Resume();

AT_API bool ATSystem_Active();

AT_API void ATSystem_Shutdown();

#endif // AT_SYSTEM_H
