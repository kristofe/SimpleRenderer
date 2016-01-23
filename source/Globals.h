/*
 *  Globals.h
 *  GWEngine_1.0
 *
 *  Created by Kristofer Schlachter on 12/26/08.
 *  Copyright 2008 Gotham Wave Games, Inc. All rights reserved.
 *
 */
#ifndef GLOBALS_H
#define GLOBALS_H

//These are overly paranoid.  You can delete a null pointer.
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

#define CLAMP(v,min,max) ((v) < (min) ? (min) : ((v) > (max) ? (max) : (v)))
#define LERP(t,a,b) ((a)+((t)*((b)-(a))))

#ifdef WIN32

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif

#ifndef NULL
#define NULL 0
#endif

#define QUOTEME(s) #s

extern unsigned int GetTimeInMilliSeconds();
//extern unsigned int GetTimeInSeconds();

typedef int ColliderID;
typedef int RigidBodyID;
typedef unsigned int GameID;
typedef unsigned int MaterialID;
typedef unsigned int MeshID;
typedef unsigned int TextureID;
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <list>
//#include "IvMathLib.h"
//#include "Thread.h"

std::string ToUpper(std::string& str);
void ToUpper(char* str);
std::vector<std::string> &SplitString(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> SplitString(const std::string &s, char delim);
std::string GetFileNameFromPath(std::string path);
std::string GetFolderNameFromPath(std::string path);
std::string GetFileExtension(std::string file);
std::string ChangeExtension(std::string file, std::string newExtension);

#define MAX_TEXTURES_PER_MATERIAL 4

#endif//GLOBALS_H