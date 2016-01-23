/*
 *  Platform_iPhone.c
 *  GWEngine_1.0
 *
 *  Created by Kristofer Schlachter on 5/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Platform_Mac.h"
//#import "Texture2D.h"
#import <Appkit/AppKit.h>
#import <QuartzCore/QuartzCore.h>
#include <sys/time.h>
//#import <OpenGLES/EAGLDisplay.h>
//#include <SDL.h>


#include <stdlib.h>
#import <OpenAL/al.h>
//#import <OpenAL/alc.h>
//#import <OpenAL/oalMacOSX_OALExtensions.h>

#import <AudioToolbox/AudioToolbox.h> // for vibration
#include "SoundEngine.h"

void GetTextureDataGL(char* pathToImage,GLubyte** imgData, int* width, int* height, int* texFormat){
	NSString* path = [[NSString alloc] initWithCString:pathToImage encoding:NSASCIIStringEncoding];
	NSImage* image = [[NSImage alloc] initWithContentsOfFile:path];
	
	*texFormat = GL_RGB;
	NSBitmapImageRep *imageBitmap = [[NSBitmapImageRep alloc] initWithData:[image TIFFRepresentation]];
	[imageBitmap retain];
	
	switch([imageBitmap samplesPerPixel])
	{
		case 4:
			*texFormat = GL_RGBA;
			break;
		case 3:
			*texFormat = GL_RGB;
			break;
		case 2:
			*texFormat = GL_LUMINANCE_ALPHA;
			break;
		case 1:
			*texFormat = GL_LUMINANCE;
			break;
		default:
			break;
			
	}
	
	*width = [imageBitmap pixelsWide];
	*height = [imageBitmap pixelsHigh];
	*imgData = [imageBitmap bitmapData];
	
	[imageBitmap release];
}

GLuint GetTexture(char* pathToImage, float& maxU, float& maxV){
	GLubyte* imgData = NULL;
	int width;
	int height;
	int texFormat;
	unsigned int tid = 0;
	GetTextureDataGL(pathToImage, &imgData, &width, &height,&texFormat);
	
	glGenTextures(1, (GLuint*)&tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, texFormat, GL_UNSIGNED_BYTE, imgData);
	
	
	maxU = 1.0;
	maxV = 1.0;
	return tid;
}	


// 
// GetTime
// Time in milliseconds since 1970
// this is probably not very stable .. check out CFAbsoluteTimeGetCurrent() instead
//
unsigned int GetTimeInMsSince1970()
{
	timeval tv;
	// The time is expressed in seconds and microseconds since midnight (0 hour), January 1, 1970.
	gettimeofday(&tv,NULL);
	// to receive milliseconds we transform the seconds to milliseconds and the microseconds to milliseconds
	// and then add them
	return (tv.tv_sec * 1000) + (unsigned int)(tv.tv_usec / 1000.0f);
}
/*
//
// This is a replacement for QueryPerformanceFrequency / QueryPerformanceCounter
// returns nanoseconds since system start
//
unsigned int GetTimeInNsSinceCPUStart()
{
	double time;
	
	time = mach_absolute_time();
	
	// this is the timebase info
    static mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    double nano = ( (double) info.numer) / ((double) info.denom);
	
	return nano * time / 1000000000.0;
}

//
// returns Ticks since system start
//
U32 GetTimeInTicksSinceCPUStart()
{
	// return value is nanoseconds
	//result = gethrtime();
	//result = _rdtsc();
	INT64BIT time;
	// This function returns its result in terms of the Mach absolute time unit. 
	// This unit is CPU dependent, so you can't just multiply it by a constant to get a real world value. 
	time = (INT64BIT) mach_absolute_time();
	
	return time;
}
*/
//void GetTextureDataGL(char* pathToImage,GLubyte** imgData, int* width, int* height){
//	CGImageRef spriteImage;
//	CGContextRef spriteContext;
//	GLubyte *spriteData;
//
//	NSString* path = [[NSString alloc] initWithCString:pathToImage encoding:NSASCIIStringEncoding];
//	// Creates a Core Graphics image from an image file
//	spriteImage = [NSImage imageNamed:path].CGImage;
//	if(spriteImage) {
//		
//		*width = CGImageGetWidth(spriteImage);
//		*height = CGImageGetHeight( spriteImage);
//		int bytesPerPixel = CGImageGetBitsPerPixel(spriteImage)/8;
//		// Allocated memory needed for the bitmap context
//		spriteData = (GLubyte *) malloc(*width * *height * bytesPerPixel);
//		// Uses the bitmatp creation function provided by the Core Graphics framework. 
//		spriteContext = CGBitmapContextCreate(spriteData , *width, *height,CGImageGetBitsPerComponent(spriteImage), CGImageGetBytesPerRow(spriteImage), CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
//		// After you create the context, you can draw the sprite image to the context.
//		
//		CGContextDrawImage(spriteContext, CGRectMake(0.0, 0.0, (float)(*width), (float)(*height)), spriteImage);
//		// You don't need the context at this point, so you need to release it to avoid memory leaks.
//		CGContextRelease(spriteContext);
//		
//		*imgData = spriteData;
//	}
//}
//
//
//
//GLuint GetTexture(char* pathToImage, float& maxU, float& maxV){
//	NSString* str = [[NSString alloc] initWithCString:pathToImage encoding:NSASCIIStringEncoding];
//	Texture2D* tex = [[Texture2D alloc] initWithImage: [NSImage imageNamed:str]];
//	maxU = [tex maxS];
//	maxV = [tex maxT];
//	return [tex name];
//}	

void GetFullFilePathFromResource(std::string& filename,std::string& buffer){
		NSString *file = [NSString stringWithCString: filename.c_str()];
		NSString *path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: file];
		buffer =  (char*)[path UTF8String];
}

void GetFullFilePathFromDocuments(std::string& filename,std::string& buffer){
	
	NSString *file = [NSString stringWithCString: filename.c_str()];
	NSString *path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: file];
	buffer = (char*)[path UTF8String];
	
	/*
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *path = [paths objectAtIndex:0];
	buffer =  (char*)[path UTF8String];
	buffer += "/";
	buffer += filename;
	 */
}
void GetResourcePath(std::string& buffer)
{
	NSString* path = [[NSBundle mainBundle] resourcePath];
	buffer = (char*)[path UTF8String];
	buffer +="/";
}

//unsigned int GetTicks()
//{
//	//return SDL_GetTicks();
//}

void SavePlayerProgress(int wordCounter, bool showPoem){};
void LoadPlayerPrefs(int* wordIndex, bool* showPoem){*wordIndex = 0; *showPoem = true;};

void interruptionListenerCallback(void *inUserData, unsigned int interruptionState){};
void initSound(){};
long long getMusicTime(){return 0;};
void startMusic(char* filename){};
void stopMusic(){};
void setMusicVolume(float volume){};
unsigned long loadSound(const char* filename){return 0;};
unsigned long loadSoundLoop(const char* loopFilename, const char* startFilename, const char* endFilename){return 0;};
void playSound(unsigned long soundID){};
void stopSound(unsigned long soundID, bool decay){};
void setSoundVolume(unsigned long soundID, float volume){};
void setSoundPitch(unsigned long soundID, float pitch){};




