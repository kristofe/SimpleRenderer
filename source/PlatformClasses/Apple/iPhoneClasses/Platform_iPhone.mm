/*
 *  Platform_iPhone.c
 *  GWEngine_1.0
 *
 *  Created by Kristofer Schlachter on 5/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#include "Platform.h"
#import "Texture2D.h"
#include "Texture.h"
#include "RenderTarget.h"
#import "PVRTexture.h"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
//#include <SDL.h>
#include <sys/time.h>
#import <OpenGLES/ES1/glext.h>
//#import <OpenGLES/EAGLDisplay.h>

#include <stdlib.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <OpenAL/oalMacOSX_OALExtensions.h>

#import <AudioToolbox/AudioToolbox.h> // for vibration
#include "SoundEngine.h"

void LoadTextureData(char* pathToImage, Texture& tex)
{
	//printf("LoadTextureData: %s\n",pathToImage);
	
	GLuint			texID;
	CGImageRef		image;
	unsigned int	*pixels;
	unsigned		width, height;
	CGContextRef	context;
	CGColorSpaceRef	colorSpace;
	
	NSString* path = [[NSString alloc] initWithCString:pathToImage encoding:NSASCIIStringEncoding];
	image = [[UIImage imageNamed:path] CGImage];
	if (image == nil)
	{
		NSLog(@"could not find %@\n", path);
		return;
	}
	width = CGImageGetWidth(image);
	height = CGImageGetHeight(image);
	
	pixels = (unsigned int*)calloc(width * height * 4, sizeof(unsigned int));
	colorSpace = CGImageGetColorSpace(image);
	context = CGBitmapContextCreate(pixels, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
	CGColorSpaceRelease(colorSpace);
	CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);

	
	int bpp = CGImageGetBitsPerPixel(image);
	printf("... got it ... width %d height %d bpp %d\n",width,height,bpp);

	tex.mWidth = width;
	tex.mHeight = height;
	tex.mPixelData = pixels;
	tex.mNumberOfColors = 4;
	tex.mHasAlpha = true;
	tex.mBitsPerPixel = tex.mNumberOfColors*8;
	tex.mDataLength = width * height * 4;
	tex.mPixelFormat = (unsigned int)GL_RGBA;
	tex.mIsCompressed = false;

	[path release];
	CGContextRelease(context);
	//CGImageRelease(image);
}



void LoadTextureDataCompressed(char* pathToImage, Texture& tex)
{
	//printf("LoadTextureDataCompressed: %s\n",tex.mShortFileName.c_str());
	
	NSString* path = [[NSString alloc] initWithCString:pathToImage encoding:NSASCIIStringEncoding];
	PVRTexture* pvr = [[PVRTexture alloc] initWithContentsOfFile:path];
	NSData* data = [pvr->_imageData objectAtIndex:0];
	tex.mWidth = pvr->_width;
	tex.mHeight = pvr->_height;
	void* pixels = malloc([data length]);
	[data getBytes:pixels];
	tex.mPixelData = (unsigned int*)pixels;
	tex.mNumberOfColors = 4;
	tex.mHasAlpha = pvr->_hasAlpha;
	tex.mHardwareID = pvr->_name;
	tex.mBitsPerPixel = 4;
	tex.mDataLength = [data length];
	tex.mPixelFormat = (unsigned int)pvr->_internalFormat;
	tex.mIsCompressed = true;
	
	[path release];
	[pvr release];
	//[data release];
	
}


void GetFullFilePathFromResource(std::string& filename,std::string& buffer){
	NSString *file = [NSString stringWithCString: filename.c_str()];
	NSString *path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: file];
	buffer =  (char*)[path UTF8String];
	//buffer = filename;
	//[file release];
	//[path release];
}

void GetFullFilePathFromDocuments(std::string& filename,std::string& buffer){
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *path = [paths objectAtIndex:0];
	buffer =  (char*)[path UTF8String];
	buffer += "/";
	buffer += filename;
	//buffer = filename;
	//[paths release];
	//[path release];
}
void GetResourcePath(std::string& buffer)
{
	NSString* path = [[NSBundle mainBundle] resourcePath];
	buffer = (char*)[path UTF8String];
	buffer +="/";
	//buffer = "";
	//[path release];
}

unsigned int GetTimeInMicroSeconds(){
	//Currently wrong!!!!
	timeval tv;
	
	gettimeofday(&tv,NULL);

	return tv.tv_usec;//Return Microseconds this is millionths of seconds multiply by 0.001 to get milliseconds
	//HOWEVER THIS IS ONLY THE AMOUNT OF MICROSECONDS PAST tv.tv_sec  So it is the fractional part of the current time!
}

unsigned int GetTimeInSeconds(){
	timeval tv;
	
	gettimeofday(&tv,NULL);
	time_t t = tv.tv_sec;
	//suseconds_t us = tv.tv_usec;
	
	return (unsigned int)t;//Return Microseconds this is millionths of seconds multiply by 0.001 to get milliseconds
}

unsigned int GetTicks()
{
	static timeval start;
	static bool started = false;
	if(!started)
	{
		gettimeofday(&start, NULL);
		started = true;
	}
	
	timeval now;
	unsigned int ticks;
	
	gettimeofday(&now, NULL);
	ticks=(now.tv_sec-start.tv_sec)*1000 +
	(now.tv_usec-start.tv_usec)/1000;
	return ticks;
}


void interruptionListenerCallback(void *inUserData, UInt32 interruptionState)
{
	/*
	 // you could do this with a cast below, but I will keep it here to make it clearer
	 YourSoundControlObject *controller = (YourSoundControlObject *) inUserData;
	 
	 if (interruptionState == kAudioSessionBeginInterruption) {
	 [controller _haltOpenALSession];
	 } else if (interruptionState == kAudioSessionEndInterruption) {
	 [controller _resumeOpenALSession];
	 }*/
}


void initSound()
{
	OSStatus result = AudioSessionInitialize(NULL, NULL, interruptionListenerCallback, NULL);
	UInt32 category = kAudioSessionCategory_AmbientSound;
	OSStatus result2 = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
	SoundEngine_Initialize(44100);
	SoundEngine_SetListenerPosition(0.0, 0.0, 0.0f);
}


long long getMusicTime()
{
	double time;
	SoundEngine_GetBackgroundMusicTime(&time);
	return time;
}

void startMusic(char* filename)
{
	// Setup sound engine. Run it at 44Khz to match the sound files
	UInt32 soundID;
	if(SoundEngine_LoadBackgroundMusicTrack(filename, true, false))
	{
		printf("**ERROR** Failed to load sound file.\n");
		return;
	}
	SoundEngine_StartBackgroundMusic();
}

void stopMusic()
{
	SoundEngine_StopBackgroundMusic(false);
}

void setMusicVolume(float volume)
{
	SoundEngine_SetBackgroundMusicVolume(volume);
}

unsigned long loadSound(const char* filename)
{
	UInt32 soundID;
	SoundEngine_LoadEffect(filename, &soundID);
	return soundID;
}

unsigned long loadSoundLoop(const char* loopFilename, const char* startFilename, const char* endFilename)
{
	UInt32 soundID;
	SoundEngine_LoadLoopingEffect(loopFilename, startFilename, endFilename, &soundID);
	return soundID;
}

void playSound(unsigned long soundID)
{
	SoundEngine_StartEffect(soundID);
}

void stopSound(unsigned long soundID, bool decay)
{
	SoundEngine_StopEffect(soundID, decay);
}

void setSoundVolume(unsigned long soundID, float volume)
{
	SoundEngine_SetEffectLevel(soundID, volume);
}

void setSoundPitch(unsigned long soundID, float pitch)
{
	SoundEngine_SetEffectPitch(soundID, pitch);
}

void vibrate()
{
	AudioServicesPlaySystemSound (kSystemSoundID_Vibrate);
}


void SavePlayerProgress(int wordIndex, bool showPoem)
{
	[[NSUserDefaults standardUserDefaults] setInteger:wordIndex forKey:@"wordIndex"];
	[[NSUserDefaults standardUserDefaults] setBool:showPoem forKey:@"showPoem"];
}

void LoadPlayerPrefs(int* wordIndex, bool* showPoem)
{
	*wordIndex = [[NSUserDefaults standardUserDefaults] integerForKey:@"wordIndex"];
	*showPoem = [[NSUserDefaults standardUserDefaults] boolForKey:@"showPoem"];
}
/*
//THIS SHOULD GO INTO THE RENDERER!!! SPLIT RendererGL into RendererGL and RendererGLES.. GLES would just override a couple of functions like the FBO creation code
void CreateFrameBufferObject(RenderTarget& rt)
{
	GLint oldFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &oldFBO);
	//GLint oldFBO_DEPTH;
	//glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &oldFBO_DEPTH);
	
	glGenTextures(1, &rt.mTargetTexture->mHardwareID);
    glBindTexture(GL_TEXTURE_2D, rt.mTargetTexture->mHardwareID);
	
	glTexImage2D(GL_TEXTURE_2D, 0, (GLuint)rt.mTargetTexture->mPixelFormat, rt.mTargetTexture->mWidth, rt.mTargetTexture->mHeight, 0, (GLuint)rt.mTargetTexture->mPixelFormat, GL_UNSIGNED_BYTE, rt.mTargetTexture->mPixelData);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR  );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	//Setup Texture Framebuffer
    glGenFramebuffersOES(1, &rt.mFBOID);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, rt.mFBOID);
    glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, rt.mTargetTexture->mHardwareID, 0);
	
	GLint backingWidth, backingHeight;
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
	glGenRenderbuffersOES(1, &rt.mFBO_DEPTHID);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, rt.mFBO_DEPTHID);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, rt.mTargetTexture->mWidth, rt.mTargetTexture->mHeight);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, rt.mFBO_DEPTHID);
   
	
	//Clear the texture bind
	//glBindTexture(GL_TEXTURE_2D,0);
	 
	// check if it worked (probably worth doing :) )
	GLuint status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE_OES:						printf("GL_FRAMEBUFFER_COMPLETE_OES\n");break;
	case 0x8CDB:										printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n");break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES:			printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES:	printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES:			printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");break;			
	case GL_FRAMEBUFFER_UNSUPPORTED_OES:					printf("GL_FRAMEBUFFER_UNSUPPORTED\n");break;	
	default:											printf("Unknown issue (%x).\n",status);break;	
	}
	
	//status = glCheckFramebufferStatusOES(GL_RENDERBUFFER_OES);
	//if (status != GL_FRAMEBUFFER_COMPLETE_OES)
	//{
	//	printf("RENDER BUFFER(DEPTH PART) OBJECT ISN'T SETUP CORRECTLY\n");
	//}
    //glBindFramebufferOES(GL_FRAMEBUFFER_OES, oldFBO);
	//glBindRenderbufferOES(GL_RENDERBUFFER_OES, oldFBO_DEPTH);
    //glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    //glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
	//glGenRenderbuffersOES(1, &texDepthbuffer);
	//glBindRenderbufferOES(GL_RENDERBUFFER_OES, texDepthbuffer);
	//glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, 128, 128);
	//glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, texDepthbuffer);
    
    //if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
    //    NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
    //    return NO;
    //}
    
}
//THIS SHOULD GO INTO THE RENDERER!!! SPLIT RendererGL into RendererGL and RendererGLES.. GLES would just override a couple of functions like the FBO creation code
GLint GetCurrrentFrameBufferObjectID()
{
	GLint fboid;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &fboid);
	return fboid;
}
//THIS SHOULD GO INTO THE RENDERER!!! SPLIT RendererGL into RendererGL and RendererGLES.. GLES would just override a couple of functions like the FBO creation code
GLint GetCurrrentRenderBufferObjectID()
{
	GLint fboid;
	glGetIntegerv(GL_RENDERBUFFER_OES, &fboid);
	return fboid;
}

//THIS SHOULD GO INTO THE RENDERER!!! SPLIT RendererGL into RendererGL and RendererGLES.. GLES would just override a couple of functions like the FBO creation code
void BindFrameBuffer(int buffID)
{
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, (GLint)buffID);
	// check if it worked (probably worth doing :) )
	GLuint status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE_OES:						printf("GL_FRAMEBUFFER_COMPLETE_OES\n");break;
		case 0x8CDB:										printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n");break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES:			printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES:	printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES:			printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");break;			
		case GL_FRAMEBUFFER_UNSUPPORTED_OES:					printf("GL_FRAMEBUFFER_UNSUPPORTED\n");break;	
		default:											printf("Unknown issue (%x).\n",status);break;	
	}
}

//THIS SHOULD GO INTO THE RENDERER!!! SPLIT RendererGL into RendererGL and RendererGLES.. GLES would just override a couple of functions like the FBO creation code
void BindRenderBuffer(int buffID)
{
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, (GLint)buffID);
}
*/
