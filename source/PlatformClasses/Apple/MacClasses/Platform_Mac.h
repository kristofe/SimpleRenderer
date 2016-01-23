/*
 *  Platform_iPhone.h
 *  GWEngine_1.0
 *
 *  Created by Kristofer Schlachter on 5/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef PLATFORM_MAC_H
#define PLATFORM_MAC_H
#include "OpenGLHelper.h"
#include "Globals.h"
#include <string>

class RenderTarget;

//extern void GetTextureDataGL(char* pathToImage,GLubyte** imgData, int* width, int* height);
extern GLuint GetTexture(char* pathToImage);
extern void GetFullFilePathFromResource(std::string& filename, std::string& returnString);
extern void GetFullFilePathFromDocuments(std::string& filename, std::string& returnString);
extern void GetResourcePath(std::string& buffer);
extern unsigned int GetTicks();
extern FILE* OpenFile(const char* path);
extern void CloseFile();
//GLuint GetTextureSDL(char* filename);

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


#endif //PLATFORM_IPHONE_H