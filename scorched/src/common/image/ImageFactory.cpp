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

#include <image/ImageFactory.h>
#include <image/ImageBitmap.h>
#include <image/ImageJpg.h>
#include <image/ImagePng.h>
#include <string>

ImageFactory::ImageFactory()
{
}

Image *ImageFactory::loadAlphaImage(
	const std::string &filename)
{
	std::string extension(filename);
	_strlwr((char *) extension.c_str());

	if (strstr(extension.c_str(), ".png"))
	{
		ImagePng *png = new ImagePng();
		png->loadFromFile(filename.c_str(), true);
		return png;
	}
	else if (strstr(extension.c_str(), ".jpg"))
	{
		ImageJpg *jpg = new ImageJpg();
		jpg->loadFromFile(filename.c_str(), true);
		return jpg;
	}

	// Failsafe !!
	ImageBitmap *bitmap = new ImageBitmap();
	bitmap->loadFromFile(filename.c_str(), true);
	return bitmap;
}
Image *ImageFactory::loadImage(
	const std::string &filename, 
	const std::string &alphafilename, 
	bool invert)
{
	std::string extension(filename);
	_strlwr((char *) extension.c_str());

	if (strstr(extension.c_str(), ".png"))
	{
		if (!alphafilename.empty())
		{
			ImagePng *png = new ImagePng();
			png->loadFromFile(filename.c_str(), alphafilename.c_str(), invert);
			return png;
		}
		ImagePng *png = new ImagePng();
		png->loadFromFile(filename.c_str());
		return png;
	} else if (strstr(extension.c_str(), ".jpg"))
	{
		if (!alphafilename.empty())
		{
			ImageJpg *jpg = new ImageJpg();
			jpg->loadFromFile(filename.c_str(), alphafilename.c_str(), invert);
			return jpg;
		}
		ImageJpg *jpg = new ImageJpg();
		jpg->loadFromFile(filename.c_str());
		return jpg;
	} 

	// Failsafe !!
	if (!alphafilename.empty())
	{
		ImageBitmap *bitmap = new ImageBitmap();
		bitmap->loadFromFile(filename.c_str(), alphafilename.c_str(), invert);
		return bitmap;
	}
	ImageBitmap *bitmap = new ImageBitmap();
	bitmap->loadFromFile(filename.c_str());
	return bitmap;
}

ImageHandle ImageFactory::loadImageHandle(
	const std::string &filename, 
	const std::string &alphafilename, 
	bool invert)
{
	Image *image = loadImage(filename, alphafilename, invert);

	ImageHandle handle(*image);
	delete image;
	return handle;
}

ImageHandle ImageFactory::loadAlphaImageHandle(
	const std::string &filename)
{
	Image *image = loadAlphaImage(filename);

	ImageHandle handle(*image);
	delete image;
	return handle;
}

ImageHandle ImageFactory::createBlank(int width, int height, bool alpha, unsigned char fill)
{
	ImageBitmap result(width, height, alpha, fill);
	return ImageHandle(result);
}

#ifndef S3D_SERVER

#include <GLEXT/GLState.h>
#include <common/Defines.h>

ImageHandle ImageFactory::grabScreen()
{
	GLint		viewport[4];		/* Current viewport */
	glGetIntegerv(GL_VIEWPORT, viewport);

	ImageHandle map = ImageFactory::createBlank(viewport[2], viewport[3], false);

	glFinish();				/* Finish all OpenGL commands */
	glPixelStorei(GL_PACK_ALIGNMENT, 4);	/* Force 4-byte alignment */
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	glReadPixels(0, 0, map.getWidth(), map.getHeight(), 
		GL_RGB, GL_UNSIGNED_BYTE, map.getBits());

	return ImageHandle(map);
}
#endif
