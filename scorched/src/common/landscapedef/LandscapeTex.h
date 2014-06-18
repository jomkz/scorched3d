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

#if !defined(__INCLUDE_LandscapeTexh_INCLUDE__)
#define __INCLUDE_LandscapeTexh_INCLUDE__

#include <landscapedef/LandscapeInclude.h>
#include <landscapedef/LandscapeGrass.h>

class LandscapeTexBase : public XMLEntryContainer
{
public:
	enum LandscapeTexBaseType
	{
		eTexLayers,
		eTexNone
	};

	LandscapeTexBase(const char *name, const char *description);
	virtual ~LandscapeTexBase();

	virtual LandscapeTexBaseType getType() = 0;
};

class LandscapeTexLayerBase : public XMLEntryContainer
{
public:
	LandscapeTexLayerBase(const char *name, const char *description);
	virtual ~LandscapeTexLayerBase();

	XMLEntryFixed worldsize;
	XMLEntryString texturename;
	XMLEntryString normaltexturename;
};

class LandscapeTexLayer : public LandscapeTexLayerBase
{
public:
	LandscapeTexLayer();
	virtual ~LandscapeTexLayer();

	XMLEntryFixed blendheightfactor;
	LandscapeGrassList grassList;
};

class LandscapeTexLayerSlope : public LandscapeTexLayerBase
{
public:
	LandscapeTexLayerSlope();
	virtual ~LandscapeTexLayerSlope();

	XMLEntryFixed blendslopestart, blendslopefactor;
};

class LandscapeTexLayerDetail : public LandscapeTexLayerBase
{
public:
	LandscapeTexLayerDetail();
	virtual ~LandscapeTexLayerDetail();

	XMLEntryFixed blendheightstartfactor;
	XMLEntryFixed blendheightendfactor;
	XMLEntryFixed blendheightfactor;
	XMLEntryInt blendnoiseoctaves;
	XMLEntryFixed blendnoisepersistence;
	XMLEntryFixed blendnoisescale;
};

class LandscapeTexLayers : public LandscapeTexBase
{
public:
	LandscapeTexLayers();
	virtual ~LandscapeTexLayers();

	XMLEntryFixed layersheight;
	XMLEntryInt layeroffsetoctaves;
	XMLEntryFixed layeroffsetpersistence;
	XMLEntryFixed layeroffsetscale;
	LandscapeTexLayer texturelayer1, texturelayer2, texturelayer3;
	LandscapeTexLayerSlope texturelayerslope;
	LandscapeTexLayerDetail texturelayerdetail;

	virtual LandscapeTexBaseType getType() { return eTexLayers; }
};

class LandscapeTexNone : public LandscapeTexBase
{
public:
	LandscapeTexNone();
	virtual ~LandscapeTexNone();

	virtual LandscapeTexBaseType getType() { return eTexNone; }
};

class LandscapeTexChoice : public XMLEntryTypeChoice<LandscapeTexBase>
{
public:
	LandscapeTexChoice();
	virtual ~LandscapeTexChoice();

	virtual LandscapeTexBase *createXMLEntry(const std::string &type, void *xmlData);
	virtual void getAllTypes(std::set<std::string> &allTypes);
};

class LandscapeTex : public LandscapeInclude
{
public:
	LandscapeTex(const char *name, const char *description, bool required);
	virtual ~LandscapeTex();
	
	LandscapeTexChoice texture;
	XMLEntryFixed waterHeight;
private:
	LandscapeTex(const LandscapeTex &other);
	LandscapeTex &operator=(LandscapeTex &other);
};

class LandscapeTextureFile : public XMLEntryRoot<LandscapeTex>
{
public:
	LandscapeTextureFile();
	virtual ~LandscapeTextureFile();
};

#endif
