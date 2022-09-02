//
//  ATEAGLView.h
//  BouncingBallGame
//
//  Created by Tristan Lewis on 7/03/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface ATEAGLView : UIView
{    
@private
	GLint backingWidth;
	GLint backingHeight;

	EAGLContext *context;

	GLuint viewRenderbuffer, viewFramebuffer;
	GLuint depthRenderbuffer;

	NSThread *mainThread;
}

- (void)BeginGameThread;
- (void)GameThread;
- (void)DisplayScreen;

@end
