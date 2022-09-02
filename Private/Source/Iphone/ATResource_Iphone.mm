/*
 *  ATResource_Iphone.cpp
 *  ArcticGameEngine
 *
 *  Created by Tristan Lewis on 25/07/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "AT.h"
#include "ATString.h"

#import <UIKit/UIKit.h>

void ATResource_GetPath(char* cBuffer, int length)
{
	NSString* readPath = [[NSBundle mainBundle] resourcePath];
	[readPath getCString:cBuffer maxLength:length encoding:NSASCIIStringEncoding];
	ATString_Concatenate(cBuffer, "/", length);
}
