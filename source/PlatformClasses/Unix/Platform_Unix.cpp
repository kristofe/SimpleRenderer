#include "Platform_Unix.h"

#include <unistd.h>
#include <stdio.h>

char* GetFullFilePathFromResource(char* filename){
    char* buffer = new char[512];
    char* wd = getcwd(NULL,0);
    printf("Working Directory = %s\n",wd);
    sprintf(buffer,"%s/../../../../../assets/%s",wd,filename);
    return buffer;
}

char* GetFullFilePathFromDocuments(char* filename){
    return filename;
}

void GetResourcePath(std::string& buffer)
{
    //buffer = [[NSBundle mainBundle] resourcePath];
}

unsigned int GetTicks()
{
    //return SDL_GetTicks();
}



