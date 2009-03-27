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

#include <common/ImageID.h>
#include <common/Defines.h>
#include <XML/XMLParser.h>

ImageID::ImageID()
{
}

ImageID::~ImageID()
{
}

bool ImageID::initFromString(
		const char *type,
		const char *imageName,
		const char *alphaName,
		bool invert)
{
	type_ = type;
	imageName_ = imageName;
	alphaName_ = alphaName;
	invert_ = invert;
	return true;
}


bool ImageID::initFromNode(const char *directory, XMLNode *imageNode)
{
	XMLNode *typeNode = 0;
	if (!imageNode->getNamedParameter("type", typeNode)) return false;
	type_ = typeNode->getContent();
	if (0 != strcmp("bmp", type_.c_str()) &&
		0 != strcmp("png", type_.c_str()))
	{
		return imageNode->returnError(
			S3D::formatStringBuffer(
				"Unknown image type \"%s\"",
				type_.c_str()));
	}

	std::string image, alpha;
	if (!imageNode->getNamedChild("image", image)) return false;
	imageName_ = std::string(directory) + std::string("/") + image;
	if (!S3D::fileExists(S3D::getModFile(imageName_.c_str())))
	{
		return imageNode->returnError(
			S3D::formatStringBuffer(
				"Image file \"%s\" does not exist",
				imageName_.c_str()));
	}

	if (imageNode->getNamedChild("alpha", alpha, false))
	{
		alphaName_ = std::string(directory) + std::string("/") + alpha;
		if (!S3D::fileExists(S3D::getModFile(alphaName_.c_str())))
		{
			return imageNode->returnError(
				S3D::formatStringBuffer(
					"Alpha file \"%s\" does not exist",
					alphaName_.c_str()));
			return false;
		}
	}

	invert_ = false;
	imageNode->getNamedChild("invert", invert_, false);

	return imageNode->failChildren();
}

const char *ImageID::getStringHash()
{ 
	hash_ = imageName_ + "-" + alphaName_ + (invert_?"A":"B");
	return hash_.c_str(); 
}
