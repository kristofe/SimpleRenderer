/*
 *  Platform_iPhone.h
 *  GWEngine_1.0
 *
 *  Created by Kristofer Schlachter on 5/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef PLATFORM_IPHONE_H
#define PLATFORM_IPHONE_H


#import <OpenGLES/ES1/gl.h>
#include <string>
#include "Globals.h"
class Texture;
class RenderTarget;

void LoadTextureDataCompressed(char* pathToImage, Texture& tex);
void LoadTextureData(char* pathToImage, Texture& tex);

//void GetTexture(char* pathToImage, unsigned int** imgData, int* width, int* height);
void GetFullFilePathFromResource(std::string& filename, std::string& returnString);
void GetFullFilePathFromDocuments(std::string& filename, std::string& returnString);
void GetResourcePath(std::string& buffer);
//GLuint GetTextureSDL(char* filename);
unsigned int GetTicks();

void SavePlayerProgress(int wordCounter, bool showPoem);
void LoadPlayerPrefs(int* wordIndex, bool* showPoem);

void interruptionListenerCallback(void *inUserData, unsigned int interruptionState);
void initSound();
long long getMusicTime();
void startMusic(char* filename);
void stopMusic();
void setMusicVolume(float volume);
unsigned long loadSound(const char* filename);
unsigned long loadSoundLoop(const char* loopFilename, const char* startFilename, const char* endFilename);
void playSound(unsigned long soundID);
void stopSound(unsigned long soundID, bool decay);
void setSoundVolume(unsigned long soundID, float volume);
void setSoundPitch(unsigned long soundID, float pitch);

/*
void CreateFrameBufferObject(RenderTarget& rt);
GLint GetCurrrentFrameBufferObjectID();
GLint GetCurrrentRenderBufferObjectID();
void BindFrameBuffer(int renderBufferID);
void BindRenderBuffer(int renderBufferID);
*/
#endif //PLATFORM_IPHONE_H