//
//  GLView.h
//  GLTeapotCocoa
//
//  Created by Kristofer Schlachter on 1/2/08.
//  Copyright 2008 Gotham Wave Games. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>
#include "Game.h"

@interface GLView : NSOpenGLView {
	//Should be in controller
	
	Game* gameEngine;
	
	NSTimer *renderTimer;
	
    bool    mZUp;               // Set to 0 or 1 to indicate whether we do Z-up or Y-up
    
    // Camera internals
    GLfloat  mCameraTheta;
    GLfloat  mCameraPhi;
    GLfloat  mCameraDistance;
    GLfloat  mCameraOffsetX;
    GLfloat  mCameraOffsetY;
    GLfloat  mCameraFOV;
	
    // dragging internals
    GLfloat  mStartCameraTheta;
    GLfloat  mStartCameraPhi;
    GLfloat  mStartCameraDistance;
    GLfloat  mStartCameraOffsetX;
    GLfloat  mStartCameraOffsetY;
	
    int      mButtonDown;
    
    float    mMouseX;
    float    mMouseY;
    float    mMouseDownX;
    float    mMouseDownY;
	
	int		mFrameWidth;
	int		mFrameHeight;
	
	unsigned int mCurrentTime;
	unsigned int mDeltaTime;
	unsigned int mLastUpdateTime; 
}

//Should be in controller;
- (void) awakeFromNib;

-(void) drawRect:(NSRect) bounds;
- (void) positionCamera;
-(void) setup3DCamera:(int)width:(int) height;
-(void) setup2DCamera:(int)width:(int)height;
- (void) resetView;
- (void) reshape;
- (void) setupRenderTimer;
-(void) dealloc;
@end
