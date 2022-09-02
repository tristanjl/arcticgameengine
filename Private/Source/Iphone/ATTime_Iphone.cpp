/*
 *  ATTime_Iphone.cpp
 *  ArcticGameEngine
 *
 *  Created by Tristan Lewis on 25/07/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include "ATTypes.h"

AT_API int64 ATTime_GetSystemTimeInMicroseconds()
{
	return (int64)(CFAbsoluteTimeGetCurrent() * 1000000.0f);
}
