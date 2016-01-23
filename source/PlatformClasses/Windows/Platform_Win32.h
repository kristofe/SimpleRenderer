#ifndef PLATFORM_WIN32_H
#define PLATFORM_WIN32_H
#include <windows.h>
#include "Globals.h"
#include <string>

#pragma warning( disable: 4996 )//sprintf not safe
#pragma warning( disable: 4018 )//unsigned signed mismatch
#pragma warning( disable: 4244 )//float to int conversion
#pragma warning( disable: 4267 )//size_t to int conversion

//extern void GetTextureDataGL(char* pathToImage,GLubyte** imgData, int* width, int* height);
//extern void GetTexture(char* pathToImage);
extern void GetFullFilePathFromResource(std::string& filename, std::string& returnString);
extern void GetFullFilePathFromDocuments(std::string& filename, std::string& returnString);
extern void GetResourcePath(std::string& buffer);
extern unsigned int	GetTicks();
//GLuint GetTextureSDL(char* filename);
void RedirectIOToConsole();


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



//FROM PLATFORM ALL


std::string GetCurrentDir();
std::vector<std::string> ListAllDirectories(std::string& directory);
void ChangeParentDir(std::string targetParent);

#endif //PLATFORM_WIN32_H