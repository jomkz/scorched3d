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

#include <image/ImageStore.h>
#include <image/ImageFactory.h>
#include <common/Defines.h>

ImageStore *ImageStore::instance_ = 0;

ImageStore *ImageStore::instance()
{
	if (!instance_)
	{
		instance_ = new ImageStore;
	}
	return instance_;
}

ImageStore::ImageStore()
{
}

ImageStore::~ImageStore()
{
}

Image ImageStore::loadImage(ImageID &imageId)
{
	std::map<std::string, Image *>::iterator findItor =
		imageMap_.find(imageId.getStringHash());
	if (findItor == imageMap_.end())
	{
		Image image = getImage(imageId);
		imageMap_[imageId.getStringHash()] = new Image(image);
		return image;
	}
	return *findItor->second;
}

Image ImageStore::getImage(ImageID &id)
{
	Image image;
	if (id.getAlphaName()[0])
	{
		image = ImageFactory::loadImage(
			S3D::getModFile(id.getImageName()),
			S3D::getModFile(id.getAlphaName()),
			id.getInvert());
	}
	else
	{
		image = ImageFactory::loadImage(
			S3D::getModFile(id.getImageName()));
	}

	return image;
}
