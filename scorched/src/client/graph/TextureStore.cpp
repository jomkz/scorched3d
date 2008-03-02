////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <graph/TextureStore.h>
#include <graph/OptionsDisplay.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLTexture.h>
#include <common/Defines.h>

TextureStore *TextureStore::instance_ = 0;

TextureStore *TextureStore::instance()
{
	if (!instance_)
	{
		instance_ = new TextureStore;
	}
	return instance_;
}

TextureStore::TextureStore()
{
}

TextureStore::~TextureStore()
{
}

GLTexture *TextureStore::loadTexture(const std::string &name, 
									 const std::string &aname, 
									 bool invert)
{
	std::string wholeName;
	wholeName += name;
	wholeName += "::";
	wholeName += aname;

	// Try to find the texture in the cache first
	std::map<std::string, GLTexture *>::iterator itor =
		skins_.find(wholeName);
	if (itor != skins_.end())
	{
		return (*itor).second;
	}

	// Load tank skin as bitmap
	Image *map = 0;
	if (aname[0])
	{
		map = ImageFactory::loadImage(name, aname, invert);
		if (!map->getBits())
		{
			S3D::dialogMessage("Scorched3D load texture", S3D::formatStringBuffer(
						  "Failed to load texture file \"%s\",\n"
						  "alpha file \"%s\"",
						  name.c_str(),
						  aname.c_str()));
			return 0;
		}
	}
	else
	{
		map = ImageFactory::loadImage(name);
		if (!map->getBits())
		{
			S3D::dialogMessage("Scorched3D load texture", S3D::formatStringBuffer(
						  "Failed to load texture file \"%s\"",
						  name.c_str()));
			return 0;
		}
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
	if (!texture->create(*map))
	{
		delete map;
		S3D::dialogMessage("Scorched3D create texture", S3D::formatStringBuffer(
					  "Failed to create texture \"%s\"",
					  name.c_str()));
		return 0;
	}
	delete map;

	skins_[wholeName] = texture;
	return texture;
}