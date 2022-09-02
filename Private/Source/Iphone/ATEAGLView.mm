//
//  EAGLView.m
//  BouncingBallGame
//
//  Created by Tristan Lewis on 7/03/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//



#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#include "AT.h"
#include "ATInput.h"
#include "ATSystem.h"
#include "ATRender.h"
#include "ATInputCursor_Internal.h"
#include "ATSystem_Internal.h"
#import "ATEAGLView.h"

#define USE_DEPTH_BUFFER 0

// A class extension to declare private methods
@interface ATEAGLView ()

@property (nonatomic, retain) EAGLContext *context;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation ATEAGLView

@synthesize context;
extern ATSystemCallback systemCallbacks[];

// You must implement this method
+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{  
	if ((self = [super initWithFrame:frame]))
	{
		// Get the layer
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

		[self setMultipleTouchEnabled:YES];

		eaglLayer.opaque = YES;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
    
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
		if (!context || ![EAGLContext setCurrentContext:context] || ![self createFramebuffer])
		{
			[self release];
			return nil;
		}
	}
  
	return self;
}


- (void)GameThread
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	glViewport(0, 0, backingWidth, backingHeight);
	ATSystem_InitialiseSystems();
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[EAGLContext setCurrentContext:context];
	while (ATSystem_Update());
	[pool release];
}

- (void)DisplayScreen
{
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}


- (BOOL)createFramebuffer
{
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	if (USE_DEPTH_BUFFER)
	{
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}
	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	return YES;
}


- (void)destroyFramebuffer
{
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	if(depthRenderbuffer)
	{
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}


- (void)BeginGameThread
{
	[NSThread detachNewThreadSelector:@selector(GameThread) toTarget:self withObject:self];
}

- (void)dealloc
{
	ATSystem_DeinitialiseSystems();

	if ([EAGLContext currentContext] == context)
	{
		[EAGLContext setCurrentContext:nil];
	}

	[context release];  
	[super dealloc];
}

#pragma mark -
#pragma mark === Touch handling  ===
#pragma mark

// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch* touch in touches)
	{
		CGPoint touchPoint = [touch locationInView:self];
        ATInputCursor_CreateTouchState(0, touchPoint.x, touchPoint.y);
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
	{
		CGPoint previousTouchPoint = [touch previousLocationInView:self];
        
        ATInputTouchData* touchData = ATInputCursor_GetTouchData(previousTouchPoint.x, previousTouchPoint.y);
		if (touchData != NULL)
		{
			CGPoint touchPoint = [touch locationInView:self];
			ATInputCursor_UpdateTouchState(touchData, ATITI_Position_X, touchPoint.x);
			ATInputCursor_UpdateTouchState(touchData, ATITI_Position_Y, touchPoint.y);
			break;
		}
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
	{
		CGPoint previousTouchPoint = [touch previousLocationInView:self];
        
        ATInputTouchData* touchData = ATInputCursor_GetTouchData(previousTouchPoint.x, previousTouchPoint.y);
		if (touchData != NULL)
		{
			CGPoint touchPoint = [touch locationInView:self];
			ATInputCursor_UpdateTouchState(touchData, ATITI_Active, 0.0f);
			ATInputCursor_UpdateTouchState(touchData, ATITI_Position_X, touchPoint.x);
			ATInputCursor_UpdateTouchState(touchData, ATITI_Position_Y, touchPoint.y);
			break;
		}
	}
}

@end
