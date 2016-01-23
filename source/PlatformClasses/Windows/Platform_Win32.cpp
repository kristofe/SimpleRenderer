#include "OpenGLHelper.h"
#include "Platform.h"
#include <direct.h>
#include "ImageUtils.h"

#include <windows.h>

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include <iostream>
#include <fstream>

/** loadTexture
* 	loads a png file into an opengl texture object, using cstdio , libpng, and opengl.
*
* 	\param filename : the png file to be loaded
* 	\param width : width of png, to be updated as a side effect of this function
* 	\param height : height of png, to be updated as a side effect of this function
*
* 	\return GLuint : an opengl texture id.  Will be 0 if there is a major error,
* 					should be validated by the client of this function.
*
*/
/*
GLuint GetTexture(char* filename)
{
int width, height;
//header for testing if it is a png
png_byte header[8];

//open file as binary
FILE *fp = fopen(filename, "rb");
if (!fp) {
return TEXTURE_LOAD_ERROR;
}

//read the header
fread(header, 1, 8, fp);

//test if png
int is_png = !png_sig_cmp(header, 0, 8);
if (!is_png) {
fclose(fp);
return TEXTURE_LOAD_ERROR;
}

//create png struct
png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
NULL, NULL);
if (!png_ptr) {
fclose(fp);
return (TEXTURE_LOAD_ERROR);
}

//create png info struct
png_infop info_ptr = png_create_info_struct(png_ptr);
if (!info_ptr) {
png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
fclose(fp);
return (TEXTURE_LOAD_ERROR);
}

//create png info struct
png_infop end_info = png_create_info_struct(png_ptr);
if (!end_info) {
png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
fclose(fp);
return (TEXTURE_LOAD_ERROR);
}

//png error stuff, not sure libpng man suggests this.
if (setjmp(png_jmpbuf(png_ptr))) {
png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
fclose(fp);
return (TEXTURE_LOAD_ERROR);
}

//init png reading
png_init_io(png_ptr, fp);

//let libpng know you already read the first 8 bytes
png_set_sig_bytes(png_ptr, 8);

// read all the info up to the image data
png_read_info(png_ptr, info_ptr);

//variables to pass to get info
int bit_depth, color_type;
png_uint_32 twidth, theight;

// get info about png
png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
NULL, NULL, NULL);

//update width and height based on png info
width = twidth;
height = theight;

// Update the png info struct.
png_read_update_info(png_ptr, info_ptr);

// Row size in bytes.
int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

// Allocate the image_data as a big block, to be given to opengl
png_byte *image_data = new png_byte[rowbytes * height];
if (!image_data) {
//clean up memory and close stuff
png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
fclose(fp);
return TEXTURE_LOAD_ERROR;
}

//row_pointers is for pointing to image_data for reading the png with libpng
png_bytep *row_pointers = new png_bytep[height];
if (!row_pointers) {
//clean up memory and close stuff
png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
delete[] image_data;
fclose(fp);
return TEXTURE_LOAD_ERROR;
}
// set the individual row_pointers to point at the correct offsets of image_data
for (int i = 0; i < height; ++i)
row_pointers[height - 1 - i] = image_data + i * rowbytes;

//read the png into image_data through row_pointers
png_read_image(png_ptr, row_pointers);

//Now generate the OpenGL texture object
GLuint texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0,
GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

//clean up memory and close stuff
png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
delete[] image_data;
delete[] row_pointers;
fclose(fp);

return texture;
}*/

/*GLuint GetTexture(char* pathToImage, float& maxU, float& maxV){
NSString* str = [[NSString alloc] initWithCString:pathToImage encoding:NSASCIIStringEncoding];
Texture2D* tex = [[Texture2D alloc] initWithImage: [UIImage imageNamed:str]];
maxU = [tex maxS];
maxV = [tex maxT];
return [tex name];
}
*/

void GetFullFilePathFromResource(std::string& filename, std::string& buffer){
  //char* buffer = new char[512];
  buffer.reserve(512);
  //char *wd=NULL;
  //wd=_getcwd(NULL,0);
  //printf("Working Directory = %s\n",wd);
  //sprintf(buffer,"%s%s","../../../assets/",filename);
  buffer = "../../../assets/";
  buffer += filename;
}

void GetFullFilePathFromDocuments(std::string& filename, std::string& buffer){
  buffer.reserve(512);
  buffer = filename;
}

void GetResourcePath(std::string& buffer)
{
  buffer.reserve(512);
  //char* buffer = new char[512];
  //char *wd=NULL;
  //wd=_getcwd(NULL,0);
  //printf("Working Directory = %s\n",wd);
  buffer = "../../../assets/";
}



void GetTexture(char* filename)
{
  /*
  SDL_Surface* sprite = IMG_Load( filename );
  if( !sprite )
  {
  printf("Could not load %s\n", filename );
  return false;
  }
  //else if( sprite->w != sprite->h )
  //{
  //    printf( "Texture w and h are not the same! Filename: %s\n w:%d\th:%d",filename, sprite->w, sprite->h);
  //    return false;
  //}
  //else if( sqrtf(sprite->w) % 2 != 0 )
  //{
  //    printf("Texture w/h are not a power of 2! w/h: " << sprite->w << ENDL;
  //}

  int texture_wh = sprite->w;

  SDL_Surface*  conv;

  // convert to 32-bit RGBA format
  conv = SDL_CreateRGBSurface( SDL_SWSURFACE, sprite->w, sprite->h, 32,
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
  0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
  #else
  0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
  #endif
  SDL_BlitSurface( sprite, 0, conv, 0);

  GLuint texture;
  glGenTextures( 1, &texture );
  glBindTexture( GL_TEXTURE_2D, texture );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR  );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  //glPixelStorei( GL_UNPACK_ROW_LENGTH, conv->pitch / conv->format->BytesPerPixel );
  glTexImage2D( GL_TEXTURE_2D, 0, 3, conv->w, conv->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, conv->pixels);
  //glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

  SDL_FreeSurface(conv);
  SDL_FreeSurface(sprite);

  return texture;*/
}



unsigned int GetTicks()
{
  return 0;
}


void RedirectIOToConsole(){

  CONSOLE_SCREEN_BUFFER_INFO coninfo;
  FILE                       *fp;

  // allocate a console for this app
  AllocConsole();

  // set the screen buffer to be big enough to let us scroll text
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
    &coninfo);
  coninfo.dwSize.Y = 500;
  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
    coninfo.dwSize);

  // redirect unbuffered STDOUT to the console
  long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
  int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stdout = *fp;
  setvbuf(stdout, NULL, _IONBF, 0);

  // redirect unbuffered STDIN to the console
  lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "r");
  *stdin = *fp;
  setvbuf(stdin, NULL, _IONBF, 0);

  // redirect unbuffered STDERR to the console
  lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stderr = *fp;
  setvbuf(stderr, NULL, _IONBF, 0);

  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
  // point to console as well
  //ios::sync_with_stdio();
}

void SavePlayerProgress(int wordCounter, bool showPoem){};
void LoadPlayerPrefs(int* wordIndex, bool* showPoem){ *wordIndex = 0; *showPoem = true; };
void interruptionListenerCallback(void *inUserData, unsigned int interruptionState){};
void initSound(){};
long long getMusicTime(){ return 0; };
void startMusic(char* filename){};
void stopMusic(){};
void setMusicVolume(float volume){};
unsigned long loadSound(const char* filename){ return 0; };
unsigned long loadSoundLoop(const char* loopFilename, const char* startFilename, const char* endFilename){ return 0; };
void playSound(unsigned long soundID){};
void stopSound(unsigned long soundID, bool decay){};
void setSoundVolume(unsigned long soundID, float volume){};
void setSoundPitch(unsigned long soundID, float pitch){};


#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
#define _GetCurrentDir _getcwd
#define _ChangeDir _chdir
#include <iostream>
#include <string>
#include <vector>

std::string GetCurrentDir()
{
  std::string s;
  char cCurrentPath[FILENAME_MAX];

  if (!_GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
  {
    s += errno;
    return s;
  }

  printf("The current working directory is %s", cCurrentPath);
  s = cCurrentPath;

  return s;
}

std::vector<std::string> ListAllDirectories(std::string& directory)
{
  std::vector<std::string> d;

#ifndef WIN32
  DIR *dir = opendir(directory.c_str());

  struct dirent *entry = readdir(dir);

  while (entry != NULL)
  {
    if (entry->d_type == DT_DIR)
    {
      if (!(
        (strcmp(entry->d_name, ".") == 0)
        ||
        (strcmp(entry->d_name, "..") == 0)
        )
        )
      {
        d.push_back(entry->d_name);
      }

    }

    entry = readdir(dir);
  }

  closedir(dir);
#endif
  WIN32_FIND_DATA ffd;
  //LARGE_INTEGER filesize;
  //TCHAR szDir[MAX_PATH];
  //size_t length_of_arg;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  DWORD dwError = 0;

  // Prepare string for use with FindFile functions.  First, copy the
  // string to a buffer, then append '\*' to the directory name.

  std::string dir = directory + "\\*";

  // Find the first file in the directory.

  std::wstring dirw(dir.begin(), dir.end());
  hFind = FindFirstFile(dirw.c_str(), &ffd);

  if (INVALID_HANDLE_VALUE == hFind)
  {
    std::cout << "ERROR FINDING " << dir << std::endl;
    return d;
  }

  // List all the files in the directory with some info about them.

  do
  {
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      std::wstring d_name = ffd.cFileName;
      if (!(
        (d_name.compare(L".") == 0)
        ||
        (d_name.compare(L"..") == 0)
        )
        )
      {
        std::string wsTemp(d_name.begin(), d_name.end());
        d.push_back(wsTemp);
      }
    }
    else
    {
      /*
      filesize.LowPart = ffd.nFileSizeLow;
      filesize.HighPart = ffd.nFileSizeHigh;
      _tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
      */
    }
  } while (FindNextFile(hFind, &ffd) != 0);

  FindClose(hFind);
  return d;
}

void ChangeParentDir(std::string targetParent)
{
  const int MAX_LEVELS_TO_SEARCH = 6;
  int levels = 0;
  //Try to descend until targetParent is visible
  std::string currDir = GetCurrentDir();
  std::cout << "Searching for " << targetParent
    << " parent directory.. starting with "
    << currDir
    << std::endl;

  std::vector<std::string> folders;
  do
  {
    folders = ListAllDirectories(currDir);
    //New C++11 range based for loop
    for (std::string& dir : folders)
    {
      if (dir.compare(targetParent) == 0)
      {
        std::cout << "Found " << currDir << "/" << targetParent
          << ".  Switching to " << currDir << "/" << dir << std::endl;
        _ChangeDir(currDir.c_str());
        return;
      }
    }
    currDir += "\\..";

    levels++;
  } while (levels < MAX_LEVELS_TO_SEARCH);

  std::cout << "Failed to find " << targetParent << std::endl;
}