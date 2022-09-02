#ifndef AT_TIME_H
#define	AT_TIME_H

AT_API int64 ATTime_GetFrameRateInMicroseconds();

AT_API int64 ATTime_GetNextFrameTimeInMicroseconds();

AT_API void ATTime_GetCurrent(int* year, int* month, int* day, int* hour, int* min, int* second);

AT_API int64 ATTime_GetSystemTimeInMicroseconds();

#endif // AT_TIME_H

