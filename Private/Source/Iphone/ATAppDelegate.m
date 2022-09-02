//
//  BouncingBallGameAppDelegate.m
//  BouncingBallGame
//
//  Created by Tristan Lewis on 7/03/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "ATAppDelegate.h"
#import "ATEAGLView.h"

@implementation ATAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	glView = [[ATEAGLView alloc] initWithFrame:window.bounds];
	[window addSubview:glView];
	[window makeKeyAndVisible];

	[glView BeginGameThread];
}


- (void)applicationWillResignActive:(UIApplication *)application
{
	//glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application
{
	//glView.animationInterval = 1.0 / 60.0;
}


- (void)dealloc
{
	[window release];
	[glView release];
	[super dealloc];
}

@end
