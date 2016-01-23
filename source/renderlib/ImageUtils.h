#ifndef _IMAGE_UTILS_H
#define _IMAGE_UTILS_H
#include "libpng/png.h"
#include "OpenGLHelper.h"
#include <SOIL.h>

namespace renderlib{
  
void read_png_file(const char* file_name,TextureProxy& tex, bool flipY = true);
void write_png_file(const char* file_name, png_bytep * row_pointers, int width, int height, png_byte color_type = PNG_COLOR_TYPE_RGBA, png_byte bit_depth = 32);
  
void read_tga_file(const char* file_name, TextureProxy& tex);
}
#endif