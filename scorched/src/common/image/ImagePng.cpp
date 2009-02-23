////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <image/ImagePng.h>
#ifdef __DARWIN__
#include <UnixImageIO/png.h>
#else
#include <png.h>
#endif

ImagePng::ImagePng() :
	width_(0), height_(0), bits_(0), alpha_(false),
	owner_(true)
{

}

ImagePng::ImagePng(int startWidth, int startHeight, bool alpha, unsigned char fill) : 
	width_(startWidth), height_(startHeight), alpha_(alpha), bits_(0),
	owner_(true)
{
	createBlankInternal(startWidth, startHeight, alpha, fill);
}

bool ImagePng::loadFromFile(const std::string &filename, 
	const std::string &alphafilename, bool invert)
{
	ImagePng bitmap;
	if (!bitmap.loadFromFile(filename)) return false;
	ImagePng alpha;
	if (!alpha.loadFromFile(alphafilename)) return false;

	if (bitmap.getBits() && alpha.getBits() && 
		bitmap.getWidth() == alpha.getWidth() &&
		bitmap.getHeight() == alpha.getHeight() &&
		bitmap.getComponents() == alpha.getComponents() &&
		bitmap.getComponents() == 3)
	{
		createBlankInternal(bitmap.getWidth(), bitmap.getHeight(), true);
		unsigned char *bbits = bitmap.getBits();
		unsigned char *abits = alpha.getBits();
		unsigned char *bits = getBits();
		for (int y=0; y<bitmap.getHeight(); y++)
		{
			for (int x=0; x<bitmap.getWidth(); x++)
			{
				bits[0] = bbits[0];
				bits[1] = bbits[1];
				bits[2] = bbits[2];

				unsigned char avg = (unsigned char)
					((
					int(abits[0]) + 
					int(abits[1]) + 
					int(abits[2])) / 3);
				if (invert)
				{
					bits[3] = (unsigned char)(255 - avg);
				}
				else
				{
					bits[3] = avg;
				}

				bbits += 3;
				abits += 3;
				bits += 4;
			}
		}
	}
	return true;
}

ImagePng::~ImagePng()
{
	clear();
}

void ImagePng::clear()
{
	if (owner_) delete [] bits_;
	bits_ = 0;
	width_ = 0;
	height_ = 0;
}

void ImagePng::createBlankInternal(int width, int height, bool alpha, unsigned char fill)
{
	clear();
	width_ = width;
	height_ = height;
	alpha_ = alpha;
	int bitsize = getComponents() * width * height;

	bits_ = new unsigned char[bitsize];
	memset(bits_, fill, bitsize);
}

bool ImagePng::loadFromFile(const std::string &filename, bool readalpha)
{
	FILE *file = fopen(filename.c_str(), "rb");
	if (!file) return false;

	int read = 0;
	char buffer[256];
	NetBuffer netBuffer;
	while (read = (int) fread(buffer, 1, 256, file))
	{
		netBuffer.addDataToBuffer(buffer, read);
	}
	fclose(file);

	if (!loadFromBuffer(netBuffer, readalpha))
	{
		Logger::log(
			S3D::formatStringBuffer("Failed to load PNG file \"%s\"", filename.c_str()));
		return false;
	}
	return true;
}

struct user_read_struct
{
	user_read_struct(NetBuffer &b) : buffer(b), position(0) {}

	int position;
	NetBuffer &buffer;
};

static void user_png_error(png_structp png_ptr, png_const_charp msg) 
{
	longjmp(png_ptr->jmpbuf,1);
}

static void user_png_warning(png_structp png_ptr, png_const_charp msg) 
{
}

static void user_read_fn(png_structp png_ptr,
        png_bytep data, png_size_t length)
{
	user_read_struct *read_io_ptr = (user_read_struct *) png_get_io_ptr(png_ptr);

	int length_left = MIN(length, read_io_ptr->buffer.getBufferUsed() - read_io_ptr->position);
	memcpy(data, &read_io_ptr->buffer.getBuffer()[read_io_ptr->position], length_left);
	read_io_ptr->position += length_left;
}

// CODE TAKEN FROM PNG SUPPLIED example.c
bool ImagePng::loadFromBuffer(NetBuffer &buffer, bool readalpha)
{
	png_structp png_ptr;
	png_infop info_ptr;

	/* Create and initialize the png_struct with the desired error handler
	* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library.  REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (png_ptr == NULL)
	{
	   return false;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
	  png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
	  return false;
	}

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng).  REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier.
	*/
	if (setjmp(png_jmpbuf(png_ptr)))
	{
	  /* Free all of the memory associated with the png_ptr and info_ptr */
	  png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	  /* If we get here, we had a problem reading the file */
	  return false;
	}
	png_set_error_fn(png_ptr, NULL, user_png_error, user_png_warning);

	/* If you are using replacement read functions, instead of calling
	* png_init_io() here you would call:
	*/
	user_read_struct read_struct(buffer);
	png_set_read_fn(png_ptr, (void *)&read_struct, user_read_fn);
	/* where user_io_ptr is a structure you want available to the callbacks */

	/*
	* If you have enough memory to read in the entire image at once,
	* and you need to specify only transforms that can be controlled
	* with one of the PNG_TRANSFORM_* bits (this presently excludes
	* dithering, filling, setting background, and doing gamma
	* adjustment), then you can read the entire image (including
	* pixels) into the info structure with this call:
	*/
	unsigned int settings = 
		PNG_TRANSFORM_PACKING | 
		PNG_TRANSFORM_STRIP_16 | 
		PNG_TRANSFORM_EXPAND;
	if (!readalpha) settings |= PNG_TRANSFORM_STRIP_ALPHA;

	png_read_png(png_ptr, info_ptr, settings, NULL);

	/* At this point you have read the entire image */

	// NEW CODE
	png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
	png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
	png_uint_32 bytes = png_get_rowbytes(png_ptr, info_ptr);
	png_uint_32 bitdepth = png_get_bit_depth(png_ptr, info_ptr);
	png_uint_32 coltype = png_get_color_type(png_ptr, info_ptr);
	png_byte channels = png_get_channels(png_ptr, info_ptr);

	if (coltype == (readalpha?PNG_COLOR_TYPE_RGB_ALPHA:PNG_COLOR_TYPE_RGB) &&
		bitdepth == 8 &&
		channels == (readalpha?4:3) &&
		(bytes / width) == (readalpha?4:3))
	{
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
		width_ = width;
		height_ = height;
		alpha_ = readalpha;

		createBlankInternal(width, height, readalpha);

		for (unsigned int h=0; h<height; h++)
		{
			memcpy(bits_ + bytes * (height - 1 - h), row_pointers[h], bytes);
		}
	}
	else
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

		Logger::log(S3D::formatStringBuffer(
			"Invalid PNG format.\n"
			"width %d, hei %d, rowbytes %d, bitd %d, "
			"colt %d, channels %d\n",
            (int)width,(int)height,(int)bytes,(int)bitdepth,
            (int)coltype,(int)channels));
		return false;
	}

	// END NEW CODE

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

	/* that's it */
	return true;
}

struct user_write_struct
{
	user_write_struct(NetBuffer &b) : buffer(b) {}

	NetBuffer &buffer;
};

static void user_write_fn(png_structp png_ptr,
        png_bytep data, png_size_t length)
{
	user_write_struct *write_io_ptr = (user_write_struct *) png_get_io_ptr(png_ptr);

	write_io_ptr->buffer.addDataToBuffer(data, length);
}

bool ImagePng::writeToBuffer(NetBuffer &buffer)
{
	png_structp png_ptr;
	png_infop info_ptr;
	int y;

	// Create structs
	png_ptr = 
		png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) return false;

	info_ptr = 
		png_create_info_struct(png_ptr);
	if (!info_ptr) return false;

	// Set Error Handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}
	png_set_error_fn(png_ptr, NULL, user_png_error, user_png_warning);

	// Set output
	user_read_struct read_struct(buffer);
	png_set_read_fn(png_ptr, (void *)&read_struct, user_read_fn);

	// png_init_io(png_ptr, fp);
	user_write_struct write_struct(buffer);
	png_set_write_fn(png_ptr, (void *)&write_struct, user_write_fn, NULL);

	// Write Header
	png_set_IHDR(png_ptr, info_ptr, 
		width_, height_,
		8, 
		PNG_COLOR_TYPE_RGB, 
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

	// Write File
	png_write_info(png_ptr, info_ptr);
	for (y=0; y<height_; y++) 
	{
		png_write_row(png_ptr, &bits_[(height_ - y - 1) * width_ * getComponents()]);
	}
	png_write_end(png_ptr, NULL);

	// Tidy
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return true;
}
