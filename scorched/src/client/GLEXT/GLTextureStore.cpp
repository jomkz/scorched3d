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

#include <graph/OptionsDisplay.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLTextureStore.h>
#include <GLEXT/GLTexture.h>
#include <common/Defines.h>

GLTextureStore *GLTextureStore::instance_ = 0;

GLTextureStore *GLTextureStore::instance()
{
	if (!instance_)
	{
		instance_ = new GLTextureStore;
	}
	return instance_;
}

GLTextureStore::GLTextureStore()
{
}

GLTextureStore::~GLTextureStore()
{
}

void GLTextureStore::addTextureReference(GLTextureReference &textureReference)
{
	references_.insert(&textureReference);
}

void GLTextureStore::removeTextureReference(GLTextureReference &textureReference)
{
	references_.erase(&textureReference);
}

GLTexture *GLTextureStore::loadTexture(const ImageID &imageID)
{
	// Try to find the texture in the cache first
	std::map<std::string, GLTexture *>::iterator itor =
		skins_.find(((ImageID &) imageID).getStringHash());
	if (itor != skins_.end())
	{
		return (*itor).second;
	}

	// Load tank skin as bitmap
	Image map = ImageFactory::loadImageID(imageID);
	if (!map.getBits())
	{
		S3D::dialogMessage("Scorched3D load texture", S3D::formatStringBuffer(
			"Failed to load texture file \"%s\",\n"
			"alpha file \"%s\"",
			((ImageID &) imageID).getImageName().c_str(),
			((ImageID &) imageID).getAlphaName().c_str()));
		return 0;
	}

	// HACK for skin creator
#ifdef dDOUBLE
	// Use smaller tank skins for texture size 0
	// Resize the bitmap
	if (OptionsDisplay::instance()->getTexSize() == 0)
	{
		map->resize(map->getWidth() / 2, 
				   map->getHeight() / 2);
	}
#endif

	// Create skin texture from bitmap
	GLTexture *texture = new GLTexture;
	if (!texture->create(map))
	{
		S3D::dialogMessage("Scorched3D create texture", S3D::formatStringBuffer(
			"Failed to create texture \"%s\"",
			((ImageID &) imageID).getImageName().c_str()));
		return 0;
	}

	skins_[((ImageID &) imageID).getStringHash()] = texture;
	return texture;
}
