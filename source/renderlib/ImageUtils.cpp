//#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include "ImageUtils.h"

namespace renderlib {
  
void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}




void read_png_file(const char* file_name,TextureProxy& tex, bool flipY)
{
	//int x, y;
	int width, height;
	//png_byte color_type;
	//png_byte bit_depth;
	//png_structp png_ptr;
	//png_infop info_ptr;
	//int number_of_passes;
	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE *fp = fopen(file_name, "rb");
	if (!fp) {
	return;
	}

	//read the header
	fread(header, 1, 8, fp);

	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) {
	fclose(fp);
	return;
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
	  NULL, NULL);
	if (!png_ptr) {
	fclose(fp);
	return;
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
	png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
	fclose(fp);
	return;
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	fclose(fp);
	return;
	}

	//png error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr))) {
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	fclose(fp);
	return;
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
	return;
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[height];
	if (!row_pointers) {
	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	fclose(fp);
	return ;
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	if(!flipY){
		for (int i = 0; i < height; ++i)
			row_pointers[i] = image_data + i * rowbytes;
	}
	else
	{
		for (int i = 0; i < height; ++i)
			row_pointers[height - 1 - i] = image_data + i * rowbytes;
	}
	
	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	/*

	  // set the individual row_pointers to point at the correct offsets of image_data
	  for (int i = 0; i < height; ++i)
		row_pointers[height - 1 - i] = (png_bytep)(image_data + i * rowbytes);

	///////////////////////////////////////////////////////////////
	*/
	//png_read_image(png_ptr, row_pointers);
	//png_set_rows(png_ptr,info_ptr,row_pointers);
	//png_read_png(png_ptr,info_ptr,/*PNG_TRANSFORM_BGR |*/ PNG_TRANSFORM_INVERT_ALPHA,NULL);
	//png_destroy_read_struct(&png_ptr, &info_ptr,NULL);

	std::shared_ptr<unsigned int> pixels((unsigned int*)malloc(rowbytes * height));
	memcpy(pixels.get(),image_data,rowbytes * height);
	tex.pixels = pixels;
	tex.width = width;
	tex.height = height;
	if(color_type == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		tex.numColorChannels = 4;
	}
	else if(color_type == PNG_COLOR_TYPE_RGB)
	{
		tex.numColorChannels = 3;
	}
	else
	{
		tex.numColorChannels = 3;
	}
	tex.hasAlpha = tex.numColorChannels==4?true:false;
	//tex.bitsPerPixel = tex.numColorChannels * bit_depth;
	if(tex.numColorChannels == 4)
	{
		tex.format  = GL_RGBA;
		tex.internalFormat  = GL_RGBA;
	}
	else
	{
		tex.format  = GL_RGB;
		tex.internalFormat  = GL_RGB;
	}
	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	fclose(fp);
}


void write_png_file(const char* file_name,png_bytep * row_pointers, int width, int height, png_byte color_type, png_byte bit_depth)
{
	//int x, y;

	//int width, height;
	//png_byte color_type;
	//png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	//int number_of_passes;

	/* create file */
	printf("writing %s\n",file_name);
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
		abort_("[write_png_file] File %s could not be opened for writing", file_name);


	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		abort_("[write_png_file] png_create_write_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);


	// write header
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, width, height,
		     bit_depth, color_type, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	//png_color_8 sig_bit;
	// optional significant bit chunk 
	// if we are dealing with a grayscale image then 
	//sig_bit.gray = true_bit_depth;
	// otherwise, if we are dealing with a color image then
	//sig_bit.red = 8;
	//sig_bit.green = 8;
	//sig_bit.blue = 8;
	// if the image has an alpha channel then
	//sig_bit.alpha = 8;
	//png_set_sBIT(png_ptr, info_ptr, &sig_bit);

	//png_set_shift(png_ptr, &sig_bit);

	/* pack pixels into bytes */
	//png_set_packing(png_ptr);

	/* swap location of alpha bytes from ARGB to RGBA */
	//png_set_swap_alpha(png_ptr);

	/* Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
	* RGB (4 channels -> 3 channels). The second parameter is not used.
	*/
	//png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);

	/* flip BGR pixels to RGB */
	//png_set_bgr(png_ptr);

	/* swap bytes of 16-bit files to most significant byte first */
	//png_set_swap(png_ptr);



	/* write bytes */
	//if (setjmp(png_jmpbuf(png_ptr)))
	//	abort_("[write_png_file] Error during writing bytes");

	//png_write_image(png_ptr, row_pointers);


	/* end write */
	//if (setjmp(png_jmpbuf(png_ptr)))
	//	abort_("[write_png_file] Error during end of write");

	//png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
	//for (y=0; y<height; y++)
	//	free(row_pointers[y]);
	//free(row_pointers);

	png_set_rows(png_ptr,info_ptr,row_pointers);
	//png_write_png(png_ptr,info_ptr,/*PNG_TRANSFORM_BGR |*/ PNG_TRANSFORM_INVERT_ALPHA,NULL);
	png_write_png(png_ptr,info_ptr,NULL,NULL);
	png_destroy_write_struct(&png_ptr, &info_ptr);


    fclose(fp);
}


// Set to byte packing, or the compiler will expand our struct, which means it won't read correctly from file
#pragma pack(1) 
struct TGAHeader
{
	char  idLength;
	char  colourMapType;
	char  dataType;
	short int colourMapOrigin;
	short int colourMapLength;
	char  colourMapDepth;
	short int xOrigin;
	short int yOrigin;
	short int width;
	short int height;
	char  bitsPerPixel;
	char  imageDescriptor;
};
// Restore packing
#pragma pack()

void read_tga_file(const char* file_name, TextureProxy& tex)
{
  //open file as binary
  FILE *file_handle = fopen(file_name, "rb");
  if (!file_handle) {
    std::cerr << "Could not find file " << file_name
            << " when trying to open for reading" << std::endl;
    return;
  }


	fseek(file_handle, 0, SEEK_END);
	size_t buffer_size = ftell(file_handle);
	fseek(file_handle, 0, SEEK_SET);

	char* buffer = new char[buffer_size];
	fread(buffer, 1, buffer_size, file_handle);
	fclose(file_handle);

	TGAHeader header;
	memcpy(&header, buffer, sizeof(TGAHeader));

	int color_mode = header.bitsPerPixel / 8;
	int image_size = header.width * header.height * 4; // We always make 32bit textures 

	if (header.dataType != 2)
	{
		std::cerr << "Could not load " << file_name << " Only 24/32bit uncompressed TGAs are supported."<< std::endl;
		return;
	}

	// Ensure we have at least 3 colors
	if (color_mode < 3)
	{
		std::cerr << "Could not load " << file_name << " Only 24 and 32bit textures are supported" << std::endl;
		return;
	}

	const char* image_src = buffer + sizeof(TGAHeader);
	std::shared_ptr<unsigned char> img(new unsigned char[image_size]);
  unsigned char* image_dest = img.get();

	// Targa is BGR, swap to RGB and flip Y axis
	for (long y = 0; y < header.height; y++)
	{
		long read_index = y * header.width * color_mode;
		long write_index = ((header.imageDescriptor & 32) != 0) ? read_index : (header.height - y - 1) * header.width * color_mode;
		for (long x = 0; x < header.width; x++)
		{
			image_dest[write_index] = image_src[read_index + 2];
			image_dest[write_index + 1] = image_src[read_index + 1];
			image_dest[write_index + 2] = image_src[read_index];
       
      //Forcing an alpha channel
			if (color_mode == 4)
				image_dest[write_index + 3] = image_src[read_index + 3];
			else
				image_dest[write_index + 3] = 255;

			write_index += 4;
			read_index += color_mode;
		}
	}

	tex.pixels = img;
	tex.width = header.width;
	tex.height = header.height;
  tex.numColorChannels = 4;
  tex.hasAlpha = true;
	//tex.bitsPerPixel = tex.numColorChannels * bit_depth;
  tex.format = GL_RGBA;
	tex.internalFormat = GL_RGBA8;
	tex.type = GL_UNSIGNED_BYTE;

	//delete[] image_dest;
	delete[] buffer;
  std::cout << "loaded file " << file_name << std::endl;

}


} //namespace renderlib