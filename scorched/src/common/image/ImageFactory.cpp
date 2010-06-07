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
#include <image/ImageBitmapFactory.h>
#include <image/ImageJpgFactory.h>
#include <image/ImagePngFactory.h>
#include <string>

ImageFactory::ImageFactory()
{
}

Image ImageFactory::loadImageID(
	const ImageID &imageId)
{
	ImageID &otherId = (ImageID &) imageId;
	if (otherId.getImageName()[0] && !otherId.getAlphaName()[0])
	{
		return loadImageInternal(otherId.getImageLocation(), otherId.getImageName(), false);
	}
	else if (!otherId.getImageName()[0] && otherId.getAlphaName()[0])
	{
		return loadImageInternal(otherId.getImageLocation(), otherId.getAlphaName(), true);
	}
	else if (otherId.getImageName()[0] && otherId.getAlphaName()[0])
	{
		Image bitmap = loadImageInternal(otherId.getImageLocation(), otherId.getImageName(), false);
		Image alpha = loadImageInternal(otherId.getImageLocation(), otherId.getAlphaName(), false);
		return combineImage(bitmap, alpha, otherId.getInvert());
	}
	return Image();
}

Image ImageFactory::loadAlphaImage(
	S3D::FileLocation imageLocation,
	const std::string &filename)
{
	ImageID imageId(imageLocation, "", filename);
	return loadImageID(imageId);
}

Image ImageFactory::loadImage(
	S3D::FileLocation imageLocation,
	const std::string &filename, 
	const std::string &alphafilename, 
	bool invert)
{
	ImageID imageId(imageLocation, filename, alphafilename, invert);
	return loadImageID(imageId);
}

Image ImageFactory::createBlank(int width, int height, bool alpha, unsigned char fill)
{
	Image result(width, height, alpha, fill);
	return result;
}

#ifndef S3D_SERVER

#include <GLEXT/GLState.h>
#include <common/Defines.h>

Image ImageFactory::grabScreen()
{
	GLint		viewport[4];		/* Current viewport */
	glGetIntegerv(GL_VIEWPORT, viewport);

	Image map = ImageFactory::createBlank(viewport[2], viewport[3], false);

	glFinish();				/* Finish all OpenGL commands */
	glPixelStorei(GL_PACK_ALIGNMENT, 4);	/* Force 4-byte alignment */
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	glReadPixels(0, 0, map.getWidth(), map.getHeight(), 
		GL_RGB, GL_UNSIGNED_BYTE, map.getBits());

	return map;
}
#endif

Image ImageFactory::loadImageInternal(S3D::FileLocation imageLocation, const std::string &filename, bool loadAlpha)
{
	std::string expandedFilename = S3D::getLocation(imageLocation, filename);
	if (strstr(filename.c_str(), ".png"))
	{
		return ImagePngFactory::loadFromFile(expandedFilename.c_str(), loadAlpha);
	}
	else if (strstr(filename.c_str(), ".jpg"))
	{
		return ImageJpgFactory::loadFromFile(expandedFilename.c_str(), loadAlpha);
	}
	else
	{
		return ImageBitmapFactory::loadFromFile(expandedFilename.c_str(), loadAlpha);
	}
}

Image ImageFactory::combineImage(Image bitmap, Image alpha, bool invert)
{
	Image result;
	if (bitmap.getBits() && alpha.getBits() && 
		bitmap.getWidth() == alpha.getWidth() &&
		bitmap.getHeight() == alpha.getHeight())
	{
		result = Image(bitmap.getWidth(), bitmap.getHeight(), true);

		unsigned char *bbits = bitmap.getBits();
		unsigned char *abits = alpha.getBits();
		unsigned char *bits = result.getBits();
		for (int y=0; y<bitmap.getHeight(); y++)
		{
			for (int x=0; x<bitmap.getWidth(); x++)
			{
				bits[0] = bbits[0];
				bits[1] = bbits[1];
				bits[2] = bbits[2];

				unsigned char avg = (unsigned char)(int(abits[0] + abits[1] + abits[2]) / 3);
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

	return result;
}
