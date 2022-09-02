/*
 *  ATDebug_Iphone.cpp
 *  ArcticGameEngine
 *
 *  Created by Tristan Lewis on 1/08/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "AT.h"
#include "ATDebug.h"
#include "ATAppDelegate.h"

#import <UIKit/UIKit.h>

@interface UIAlertView (Synthesize)

- (void)setRunsModal:(BOOL)modal;

- (void)setNumberOfRows:(int)num;

@end

AT_API bool ATDebug_DisplayAssertPlatformSpecific(const char *outputString)
{
	NSString *nsOutString = [NSString stringWithUTF8String:outputString];
	UIAlertView *assertDisplay = [[UIAlertView alloc] initWithTitle:@"Assert" message:nsOutString delegate:nil cancelButtonTitle:@"Ignore" otherButtonTitles:nil];
	[assertDisplay addButtonWithTitle:@"Abort"];
	[assertDisplay addButtonWithTitle:@"Retry"];

	[assertDisplay setRunsModal:YES];

	[((ATAppDelegate *)[UIApplication sharedApplication].delegate).glView addSubview:assertDisplay];
	[assertDisplay show];
	[assertDisplay autorelease];

	return true;
}
