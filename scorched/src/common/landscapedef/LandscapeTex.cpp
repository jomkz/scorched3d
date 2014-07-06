////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDescriptions.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

LandscapeTexBase::LandscapeTexBase(const char *name, const char *description) :
	XMLEntryContainer(name, description)
{
}

LandscapeTexBase::~LandscapeTexBase()
{
}

LandscapeTexLayerBase::LandscapeTexLayerBase(const char *name, const char *description) :
	XMLEntryContainer(name, description),
	worldsize("The scale of this texture layer.  i.e. the size in world units that this texture image will cover."
		"The texture will be tiled if the world size is smaller than the landscape size."),
	texturename("The name of the texture to be used for this layer."
		"This texture must have already been loaded into the available ogre resources."),
	normaltexturename("The name of the normal texture to be used for this layer."
		"This texture must have already been loaded into the available ogre resources.")
{
	addChildXMLEntry("worldsize", &worldsize);
	addChildXMLEntry("texturename", &texturename);
	addChildXMLEntry("normaltexturename", &normaltexturename);
}

LandscapeTexLayerBase::~LandscapeTexLayerBase()
{
}

LandscapeTexLayer::LandscapeTexLayer() :
	LandscapeTexLayerBase("LandscapeTextureLayer",
		"Defines a texture layer for the landscape.  Each layer is generally associated with a band of height on the terrain."
		"For example one layer could define the shore (the layer closest to the water)"
		" and another layer could define the snow at the top of the mountains (the highest layer)."),
	blendheightfactor("Each texture height layer is blended this proportion into the next height band.")
{
	addChildXMLEntry("blendheightfactor", &blendheightfactor);
	addChildXMLEntry("grass", &grassList);
}

LandscapeTexLayer::~LandscapeTexLayer()
{
}

LandscapeTexLayerSlope::LandscapeTexLayerSlope() :
	LandscapeTexLayerBase("LandscapeTextureLayerSlope",
		"Defines a slope texture layer for the landscape.  The slope layer is applied to areas of the landscape that have a large gradient (a big slope)"),
	blendslopestart("Considers the terrain to be on a slope if the upward normal is greater than this value."),
	blendslopefactor("Each slope layer is blended this proportion into the next height band.")
{
	addChildXMLEntry("blendslopestart", &blendslopestart);
	addChildXMLEntry("blendslopefactor", &blendslopefactor);
}

LandscapeTexLayerSlope::~LandscapeTexLayerSlope()
{
}

LandscapeTexLayerDetail::LandscapeTexLayerDetail() :
	LandscapeTexLayerBase("LandscapeTextureLayerDetail",
		"Defines a detail texture layer for the landscape.  The detail texture is applied to random parts of the landscape regardless of height."
		"This helps to hide the uniformity of the texture banding."),
	blendheightstartfactor("The height to start applying the detail texture."),
	blendheightendfactor("The height to stop applying the detail texture (Note: this is subtracted from the actual layer height)"),
	blendheightfactor("The detail is blended this proportion into the next height band."),
	blendnoiseoctaves("Parameter for the noise that determines the when to show the detail texture. "
		" Adding more octaves increases the detail of Perlin noise, with the added drawback of increasing the calculation time."),
	blendnoisepersistence("Parameter for the noise that determines the when to show the detail texture. "
		"  A multiplier that determines how quickly the amplitudes diminish for each successive octave in a Perlin-noise function. "
		"  i.e. how much detail from each octave is left in the final result"),
	blendnoisescale("Parameter for the noise that determines the when to show the detail texture. "
		"  A multiplier that determines how much the noise is scaled, more scale causes the noise to become less random")
{
	addChildXMLEntry("blendheightstartfactor", &blendheightstartfactor);
	addChildXMLEntry("blendheightendfactor", &blendheightendfactor);
	addChildXMLEntry("blendheightfactor", &blendheightfactor);
	addChildXMLEntry("blendnoiseoctaves", &blendnoiseoctaves);
	addChildXMLEntry("blendnoisepersistence", &blendnoisepersistence);
	addChildXMLEntry("blendnoisescale", &blendnoisescale);
}

LandscapeTexLayerDetail::~LandscapeTexLayerDetail()
{
}

LandscapeTexLayers::LandscapeTexLayers() :
	LandscapeTexBase("LandscapeTextureLayers", 
		"Defines a set of texture layers for the landscape."
		"Each layer is associated with a different terrain characteristic."
		"Some layers are associated with height, for example one layer could define the shore (the layer closest to the water)"
		" and another layer could define the snow at the top of the mountains (the highest layer)."
		"Other layers are associated with gradient, for example this layer can be used for clff faces."),
	layersheight("Which height on the landscape will start the last height layer."),
	layeroffsetoctaves("Parameter for the noise that gets added to the height layers to prevent uniform height bands. "
		" Adding more octaves increases the detail of Perlin noise, with the added drawback of increasing the calculation time."),
	layeroffsetpersistence("Parameter for the noise that gets added to the height layers to prevent uniform height bands. "
		"  A multiplier that determines how quickly the amplitudes diminish for each successive octave in a Perlin-noise function. "
		"  i.e. how much detail from each octave is left in the final result"),
	layeroffsetscale("Parameter for the noise that gets added to the height layers to prevent uniform height bands. "
		"  A multiplier that determines how much the noise is scaled, more scale causes the noise to become less random")
{
	addChildXMLEntry("layersheight", &layersheight);
	addChildXMLEntry("layeroffsetoctaves", &layeroffsetoctaves);
	addChildXMLEntry("layeroffsetpersistence", &layeroffsetpersistence);
	addChildXMLEntry("layeroffsetscale", &layeroffsetscale);
	addChildXMLEntry("texturelayer1", &texturelayer1,
		"texturelayer2", &texturelayer2,
		"texturelayer3", &texturelayer3,
		"texturelayerslope", &texturelayerslope,
		"texturelayerdetail", &texturelayerdetail);
}

LandscapeTexLayers::~LandscapeTexLayers()
{
}

LandscapeTexNone::LandscapeTexNone() :
	LandscapeTexBase("LandscapeTextureNone",
		"This landscape has no texture")
{
}

LandscapeTexNone::~LandscapeTexNone()
{
}

LandscapeTexChoice::LandscapeTexChoice() :
	XMLEntryTypeChoice<LandscapeTexBase>("LandscapeTextureChoice",
		"Defines the landscape visuals (the textures)")
{
}

LandscapeTexChoice::~LandscapeTexChoice()
{
}

LandscapeTexBase *LandscapeTexChoice::createXMLEntry(const std::string &type, void *xmlData)
{
	if (0 == strcmp(type.c_str(), "layers")) return new LandscapeTexLayers;
	if (0 == strcmp(type.c_str(), "none")) return new LandscapeTexNone;
	S3D::dialogMessage("LandscapeTextureChoice", S3D::formatStringBuffer("Unknown texture type %s", type));
	return 0;
}

void LandscapeTexChoice::getAllTypes(std::set<std::string> &allTypes)
{
	allTypes.insert("layers");
	allTypes.insert("none");
}

LandscapeTex::LandscapeTex(const char *name, const char *description, bool required) : 
	LandscapeInclude(name, description, required)
{
	addChildXMLEntry("texture", &texture);
	addChildXMLEntry("water", &water);
	addChildXMLEntry("sky", &sky);
	addChildXMLEntry("tree", &trees);
}

LandscapeTex::~LandscapeTex()
{
}

LandscapeTextureFile::LandscapeTextureFile() :
		XMLEntryRoot<LandscapeTex>(S3D::eModLocation, "user_defined", 
			"tex",
			"LandscapeTexture", 
			"A landscape/scene definition, usualy related to the visual aspects of the landscape", true)
{
}

LandscapeTextureFile::~LandscapeTextureFile()
{
}
