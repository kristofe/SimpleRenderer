#ifndef _IMAGE_UTILS_H
#define _IMAGE_UTILS_H
#include "libpng/png.h"
#include "OpenGLHelper.h"
#include <SOIL.h>

namespace renderlib{
  
void read_png_file(const char* file_name,TextureProxy& tex, bool flipY = true);
void write_png_file(const char* file_name, png_bytep * row_pointers, int width, int height, png_byte color_type = PNG_COLOR_TYPE_RGBA, png_byte bit_depth = 32);
  
void read_tga_file(const char* file_name, TextureProxy& tex);

static void saveScreenShotTGA(const std::string& filename, int w, int h)
{
    /*
    std::vector< unsigned char > buf( w * h * 3 );
     glPixelStorei( GL_PACK_ALIGNMENT, 1 );
     glReadPixels( 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buf[0] );
     
     int err = SOIL_save_image
     (
     filename.c_str(),
     SOIL_SAVE_TYPE
     w, h, 3,
     &buf[0]
     );
     */
    
    SOIL_save_screenshot ( filename.c_str(), SOIL_SAVE_TYPE_TGA, 0, 0, w, h);
}
  
}
#endif