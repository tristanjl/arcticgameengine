//
//  BouncingBallGameAppDelegate.h
//  BouncingBallGame
//
//  Created by Tristan Lewis on 7/03/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ATEAGLView;

@interface ATAppDelegate : NSObject <UIApplicationDelegate> {
	UIWindow *window;
	ATEAGLView *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet ATEAGLView *glView;

@end

