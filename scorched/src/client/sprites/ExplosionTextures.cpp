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

#include <image/ImageFactory.h>
#include <XML/XMLFile.h>
#include <sprites/ExplosionTextures.h>
#include <landscape/Landscape.h>
#include <lang/LangResource.h>
#include <common/Defines.h>
#include <stdio.h>

ExplosionTextures *ExplosionTextures::instance_ = 0;

ExplosionTextures *ExplosionTextures::instance()
{
	if (!instance_)
	{
		instance_ = new ExplosionTextures;
	}

	return instance_;
}

ExplosionTextures::ExplosionTextures()
{

}

ExplosionTextures::~ExplosionTextures()
{

}

bool ExplosionTextures::addTextureToSet(GLTextureSet &set,
										const char *texPath)
{
	ImageHandle bitmap =
		ImageFactory::loadImageHandle(
			(char *) texPath, (char *) texPath, false);
	GLTexture *texture = new GLTexture;

	if (!texture->create(bitmap)) return false;
	set.addTexture(texture);
	return true;
}

bool ExplosionTextures::createTextures(ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("EXPLOSION_TEXTURES", "Explosion Textures"));

	std::string file1 = S3D::getDataFile("data/textures/smoke01.bmp");
	ImageHandle bitmap = ImageFactory::loadImageHandle(file1.c_str(), file1.c_str(), false);
	smokeTexture.create(bitmap);
	DIALOG_ASSERT(smokeTexture.textureValid());

	std::string file2 = S3D::getDataFile("data/textures/smoke02.bmp");
	ImageHandle bitmap2 = ImageFactory::loadImageHandle(file2.c_str(), file2.c_str(), false);
	smokeTexture2.create(bitmap2);
	DIALOG_ASSERT(smokeTexture2.textureValid());

	std::string file3 = S3D::getDataFile("data/textures/particle.bmp");
	ImageHandle bitmap3 = ImageFactory::loadImageHandle(file3.c_str(), file3.c_str(), false);
	particleTexture.create(bitmap3);
	DIALOG_ASSERT(particleTexture.textureValid());

	ImageHandle talkBitmap = ImageFactory::loadAlphaImageHandle(S3D::getDataFile("data/textures/talk.bmp"));
	talkTexture.create(talkBitmap);
	DIALOG_ASSERT(talkTexture.textureValid());

	std::string file5 = S3D::getDataFile("data/textures/rain.bmp");
	std::string file5m = S3D::getDataFile("data/textures/rainm.bmp");
	ImageHandle bitmap5 = ImageFactory::loadImageHandle(file5m.c_str(), file5.c_str(), false);
	rainTexture.create(bitmap5);
	DIALOG_ASSERT(rainTexture.textureValid());

	std::string file6 = S3D::getDataFile("data/textures/snow.bmp");
	std::string file6m = S3D::getDataFile("data/textures/snowm.bmp");
	ImageHandle bitmap6 = ImageFactory::loadImageHandle(file6.c_str(), file6m.c_str(), false);
	snowTexture.create(bitmap6);
	DIALOG_ASSERT(snowTexture.textureValid());

	XMLFile file;
	if (!file.readFile(S3D::getDataFile("data/textureset.xml")))
	{
		S3D::dialogMessage("ExplosionTextures", S3D::formatStringBuffer(
					  "Failed to parse \"%s\"\n%s", 
					  "data/textureset.xml",
					  file.getParserError()));
		return false;
	}
	if (!file.getRootNode())
	{
		S3D::dialogMessage("ExplosionTextures", S3D::formatStringBuffer(
					  "Failed to find explosion textures definition file \"%s\"",
					  "data/textureset.xml"));
		return false;		
	}

    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 childrenItor++)
    {
		// Check if it is a texture set
        XMLNode *currentNode = (*childrenItor);
		if (strcmp(currentNode->getName(), "textureset"))
		{
			S3D::dialogMessage("ExplosionTextures",
						  "Failed to find textureset node");
			return false;
		}
		XMLNode *name = 0;
		if (!currentNode->getNamedParameter("name", name)) return false;
		const char *setName = name->getContent();

		// Create and store the new texture set
		GLTextureSet *set = new GLTextureSet();
		textureSets[setName] = set;

		// Load all the textures
		std::list<XMLNode *>::iterator textureItor;
		std::list<XMLNode *> &textures = currentNode->getChildren();
		for (textureItor = textures.begin();
			textureItor != textures.end();
			textureItor++)
		{
			// Check if it is a texture
			XMLNode *currentTexture = (*textureItor);
			if (strcmp(currentTexture->getName(), "texture"))
			{
				S3D::dialogMessage("ExplosionTextures",
							"Failed to find texture sub-node");
				return false;
			}

			// Load texture
			std::string texFile = 
				S3D::getDataFile(S3D::formatStringBuffer("data/%s", currentTexture->getContent()));
			if (!addTextureToSet(*set, texFile.c_str()))
			{
				S3D::dialogMessage("ExplosionTextures", S3D::formatStringBuffer(
							"Failed to load texture %s",
							texFile.c_str()));
				return false;
			}
		}
	}

	return true;
}

GLTextureSet *ExplosionTextures::getTextureSetByName(const char *name)
{
	DIALOG_ASSERT(!textureSets.empty());

	GLTextureSet *result = (*textureSets.begin()).second;
	std::map<std::string, GLTextureSet*>::iterator itor =
		textureSets.find(name);
	if (itor != textureSets.end())
	{
		result = (*itor).second;
	}
	return result;
}

Image &ExplosionTextures::getScorchBitmap(const char *name)
{
	if (name[0])
	{
		std::map<std::string, Image*>::iterator findItor =
			scorchedBitmaps.find(name);
		if (findItor != scorchedBitmaps.end())
		{
			return *(*findItor).second;
		}

		std::string fileName = S3D::getDataFile(name);
		if (S3D::fileExists(fileName))
		{
			Image *map = ImageFactory::loadImage(fileName);
			scorchedBitmaps[name] = map;
			return *map;
		}
	}
	return Landscape::instance()->getScorchMap();
}
