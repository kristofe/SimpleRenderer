#ifndef _IMAGE_UTILS_H
#define _IMAGE_UTILS_H
#include "libpng/png.h"
#include "OpenGLHelper.h"
#include <SOIL.h>

namespace renderlib{
  
void read_png_file(const char* file_name,TextureProxy& tex, bool flipY = true);
void write_png_file(const char* file_name, png_bytep * row_pointers, int width, int height, png_byte color_type = PNG_COLOR_TYPE_RGB, png_byte bit_depth = 8);
  
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

static void saveScreenShotPNG(const std::string& filename, int w, int h)
{
    std::vector< unsigned char > buf( w * h * 3 );
     glPixelStorei( GL_PACK_ALIGNMENT, 1 );
     glReadPixels( 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buf[0] );

	 //Now we have to create h rows of w unsigned chars
	 png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep)*h);
	 for (int y = 0; y < h; y++)
	 {
		 rows[y] = (png_byte*)malloc(sizeof(png_byte)*w*3);
		 for (int x = 0; x < w*3; x++)
		 {
			 rows[y][x] = buf[((h-1)-y)*w*3 + x];
		 }
	 }
     
	 write_png_file(filename.c_str(), rows, w, h, (png_byte)PNG_COLOR_TYPE_RGB, (png_byte)8);

	 for (int y = 0; y < h; y++)
		 free(rows[y]);

	 free(rows);
	 /*
     int err = SOIL_save_image
     (
     filename.c_str(),
     SOIL_SAVE_TYPE
     w, h, 3,
     &buf[0]
     );
    
    SOIL_save_screenshot ( filename.c_str(), SOIL_SAVE_TYPE_TGA, 0, 0, w, h);
     */
}
  
}
#endif