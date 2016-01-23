/*
 *  Globals.cpp
 *  GWEngine_1.0
 *
 *  Created by Kristofer Schlachter on 12/26/08.
 *  Copyright 2008 Gotham Wave Games, Inc. All rights reserved.
 *
 */
//#include <sys/time.h>
//#include "Game.h"
#include "Globals.h"
#include <algorithm>
#include <cctype>
#include <string>

unsigned int GetTimeInMilliSeconds(){
	//timeval tv;
	
	//gettimeofday(&tv,NULL);
	
	//return tv.tv_usec;
	return 0;
}

void ToUpper(char* str)
{
	int i =0;
	while(str[i] != 0)
	{
		char ch = str[i];
		if(ch >= 'a' && ch <= 'z')
		{
			str[i] = ch + 'A' - 'a';
			
		}
		i++;
		
	}

}

std::string ToUpper(std::string& str)
{
	std::string s = str;
	/*int count  = strlen(str.c_str());
	char* buffer = new char[count];
	strcpy(buffer,str.c_str());
	ToUpper(buffer);
	s = buffer;
	SAFE_DELETE_ARRAY(buffer);*/
	std::transform(s.begin(),s.end(),s.begin(),(int(*)(int))std::toupper);
	return s;//this should do a copy so it is safe
}


std::vector<std::string> &SplitString(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> SplitString(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return SplitString(s, delim, elems);
}

std::string GetFileNameFromPath(std::string path)
{
	size_t found;
	found=path.find_last_of("/\\");
	return path.substr(found+1);

}

std::string GetFolderNameFromPath(std::string path)
{
	size_t found;
	found=path.find_last_of("/\\");
	return path.substr(0,found);
}

std::string GetFileExtension(std::string file)
{
	size_t found;
	found=file.find_last_of(".");
	return file.substr(found+1);
}

std::string ChangeExtension(std::string file, std::string newExtension)
{
	size_t found;
	found=file.find_last_of(".");
	std::string ret =  file.substr(found+1);
	ret += newExtension;
	return ret;
}
//unsigned int GetTimeInSeconds(){
//	timeval tv;
//	
//	//gettimeofday(&tv,NULL);
//	time_t t = 0;//tv.tv_sec;
//	//suseconds_t us = tv.tv_usec;
//	
//	return (unsigned int)t;//Return Microseconds this is millionths of seconds multiply by 0.001 to get milliseconds
//}




