//
//  GLView.m
//  GLTeapotCocoa
//
//  Created by Kristofer Schlachter on 1/2/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "GLView.h"
#import "Platform_Mac.h"
#include "IntroScene.h"
#include "Game.h"

static void PerspectiveMatrix(float fovy, float aspect, float zNear, float zFar)
{
    float f = 1.0f / tanf(fovy * (M_PI / 360.f));
    float z1 = (zFar + zNear) / (zNear - zFar);
    float z2 = (2 * zFar * zNear) / (zNear - zFar);
	
    float m[] =
    {
        f / aspect,  0,      0,      0,
        0,           f,      0,      0,
        0,           0,     z1,     -1,
        0,           0,     z2,      0
    };
	
    glMultMatrixf(m);
}

@implementation GLView

- (id)initWithFrame:(NSRect)frame {
	
    return  [ super initWithFrame:frame ];
}

- (void) awakeFromNib
{
	//Should be in controller
	[ NSApp setDelegate:self ];   // We want delegate notifications
	renderTimer = nil;
	//[ glWindow makeFirstResponder:self ];
	[ self setupRenderTimer ];
	
	mZUp = 0;
	gameEngine  = (Game*)GetGame();
	[self resetView];
	[ [ self openGLContext ] makeCurrentContext ];
	[ self reshape ];
	
	IntroScene* ve = new IntroScene();
	gameEngine->setSim(ve);
	gameEngine->start();
	
	
	GLuint name;
	float maxU, maxV;
	
//	char* fullpath = (char *)getResourcePath("Background.png");
//	name = GetTexture(fullpath,maxU,maxV);
//	
//	//char* songpath = (char *)getResourcePath("wave.mp3");
//	char* songpath = (char *)getResourcePath("Lucy In The Sky With Diamonds.mp3");
//	ve->preprocessSong(songpath);
//	ve->setTexture(name, maxU, maxV);
	
	//gameEngine->start(songpath);
	//gameEngine->setTexture(name,maxU,maxV);
}

/*
 * Setup timer to update the OpenGL view.
 */
- (void) setupRenderTimer
{
	NSTimeInterval timeInterval = 0.033;
	
	renderTimer = [ [ NSTimer scheduledTimerWithTimeInterval:timeInterval
													  target:self
													selector:@selector( updateGLView: )
													userInfo:nil repeats:YES ] retain ];
	[ [ NSRunLoop currentRunLoop ] addTimer:renderTimer
									forMode:NSEventTrackingRunLoopMode ];
	[ [ NSRunLoop currentRunLoop ] addTimer:renderTimer
									forMode:NSModalPanelRunLoopMode ];
}


/*
 * Called by the rendering timer.
 */
- (void) updateGLView:(NSTimer *)timer
{
	[self drawRect: self.frame ];
}  



- (void)drawRect:(NSRect)rect {
    // Drawing code here.
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//The dt parameter should really come from a clock difference not a fixed amount
	mCurrentTime = GetTimeInMilliSeconds();
	mDeltaTime = mLastUpdateTime - mCurrentTime;
	mLastUpdateTime = mCurrentTime;
	gameEngine->update();
	gameEngine->render();
	gameEngine->lateUpdate();
	
    glFlush();
}

- (void) reshape
{ 	
	NSRect sceneBounds;
	
	[ [ self openGLContext ] update ];
	sceneBounds = [ self bounds ];
	// Reset current viewport
	glViewport( 0, 0, sceneBounds.size.width, sceneBounds.size.height );
	
	[self positionCamera];
	
	

}

- (void) resetView
{
	NSSize size = self.frame.size;
	
	mFrameWidth = size.width;
	mFrameHeight = size.height;
	
    mCameraTheta = 0;
    mCameraPhi = 0;
    mCameraDistance = 0.5;
    mCameraOffsetX = 0;
    mCameraOffsetY = 0;
    mCameraFOV = 50;
	
    mStartCameraTheta = 0;
    mStartCameraPhi = 0;
    mStartCameraDistance = 0;
    mStartCameraOffsetX = 0;
    mStartCameraOffsetY = 0;
	
    mMouseX = 0;
    mMouseY = 0;
    mMouseDownX = 0;
    mMouseDownY = 0;
}

- (void) positionCamera
{
	//eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_WIDTH,  &tw);
    //eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_HEIGHT, &th);
	NSRect sceneBounds;
	sceneBounds = [ self bounds ];
    
    //[self setup3DCamera:sceneBounds.size.width:sceneBounds.size.height];
	[self setup2DCamera:sceneBounds.size.width:sceneBounds.size.height];
}

-(void) setup3DCamera:(int)width:(int) height
{
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	PerspectiveMatrix(mCameraFOV, (float) width / (float)height, 0.1f, 1000);
    if (mZUp)
        glRotatef(-90, 1, 0, 0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (mZUp)
        glTranslatef(mCameraOffsetX, mCameraDistance, mCameraOffsetY);
    else
        glTranslatef(mCameraOffsetX, mCameraOffsetY, -mCameraDistance);
	
    glRotatef(mCameraPhi,   1, 0, 0);
    glRotatef(mCameraTheta, 0, 0, 1);	
}

-(void) setup2DCamera:(int)width:(int)height
{
	//eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_WIDTH,  &tw);
    //eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_HEIGHT, &th);

	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	glOrtho(0, width, 0, height,-10,10);
	
	if (mZUp)
        glRotatef(-90, 1, 0, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glScalef(width,height,1);

	 if (mZUp)
	 	 glTranslatef(mCameraOffsetX, mCameraDistance, mCameraOffsetY);
	 else
	 	 glTranslatef(mCameraOffsetX, mCameraOffsetY, -mCameraDistance);
	 
	 glRotatef(mCameraPhi,   1, 0, 0);
	 glRotatef(mCameraTheta, 0, 0, 1);

}

- (void) mouseDown:(NSEvent*)event
{
   
	NSPoint location = [event locationInWindow];
    float x = location.x;//NSRectGetMinX(location);
    float y = location.y;//NSRectGetMinY(location);
    
    int startCount = (int) [event deltaX];
    if (startCount == 2)    // simultaneous double-finger tap
        [self resetView];
	
    int count = (int) [event deltaY];
	
    mMouseDownX = x;
    mMouseDownY = y;
    mButtonDown = count;
    
	gameEngine->mouseDown(x/((float)self.frame.size.width),y/((float)self.frame.size.height), count);

	
}

- (void)mouseUp:(NSEvent*)event
{
	NSPoint location = [event locationInWindow];
    float x = location.x;//NSRectGetMinX(location);
    float y = location.y;//NSRectGetMinY(location);
	
	mButtonDown = 0;
	
	gameEngine->mouseUp(x/((float)self.frame.size.width),y/((float)self.frame.size.height), 0);
	
}


- (void)mouseDragged:(NSEvent*)event
{
	
    NSPoint location = [event locationInWindow];
    float x = location.x;//.origin.x;
    float y = location.y;//.origin.y;
    int count = (int) [event deltaY];
	

    gameEngine->mouseDragged(x/((float)self.frame.size.width),y/((float)self.frame.size.height),0,0, count);

}

-(void)dealloc
{
	[super dealloc];
	if( renderTimer != nil && [ renderTimer isValid ] )
		[ renderTimer invalidate ];	
}
@end

