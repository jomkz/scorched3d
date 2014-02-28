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

#if !defined(__INCLUDE_LandscapeDefnTankStarth_INCLUDE__)
#define __INCLUDE_LandscapeDefnTankStarth_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>
#include <image/Image.h>

class LandscapeDefnTankStartMinMax : public XMLEntryContainer
{
public:
	LandscapeDefnTankStartMinMax(const char *description);
	virtual ~LandscapeDefnTankStartMinMax();

	XMLEntryFixed min, max;
};

class LandscapeDefnTankStartPositionList : public XMLEntryList<XMLEntryFixedVector>
{
public:
	LandscapeDefnTankStartPositionList();
	virtual ~LandscapeDefnTankStartPositionList();

	virtual XMLEntryFixedVector *createXMLEntry();
};

class RandomGenerator;
class ScorchedContext;
class LandscapeDefnTankStart : public XMLEntryContainer
{
public:
	enum DefnTankStartType
	{
		eHeight,
		ePositional
	};

	LandscapeDefnTankStart(const char *name, const char *description);
	virtual ~LandscapeDefnTankStart();

	virtual DefnTankStartType getType() = 0;

	virtual FixedVector placeTank(unsigned int playerId, int team,
		ScorchedContext &context, RandomGenerator &generator) = 0;
};

class LandscapeDefnTankStartChoice : public XMLEntryTypeChoice<LandscapeDefnTankStart>
{
public:
	LandscapeDefnTankStartChoice();
	virtual ~LandscapeDefnTankStartChoice();

	virtual LandscapeDefnTankStart *createXMLEntry(const std::string &type);
	virtual void getAllTypes(std::set<std::string> &allTypes);
};

class LandscapeDefnTankStartHeight : public LandscapeDefnTankStart
{
public:
	LandscapeDefnTankStartHeight();
	virtual ~LandscapeDefnTankStartHeight();

	virtual bool readXML(XMLNode *node);

	virtual DefnTankStartType getType() { return eHeight; }
	virtual FixedVector placeTank(unsigned int playerId, int team,
		ScorchedContext &context, RandomGenerator &generator);

protected:
	XMLEntryFixed flatness;
	XMLEntryFixed startcloseness;
	LandscapeDefnTankStartMinMax height;
	XMLEntryFile startmask;
	Image tankMask;
};

class LandscapeDefnTankStartPositional : public LandscapeDefnTankStart
{
public:
	LandscapeDefnTankStartPositional();
	virtual ~LandscapeDefnTankStartPositional();

	virtual bool readXML(XMLNode *node);

	virtual DefnTankStartType getType() { return ePositional; }
	virtual FixedVector placeTank(unsigned int playerId, int team,
		ScorchedContext &context, RandomGenerator &generator);

protected:
	XMLEntryFixed flatness;
	LandscapeDefnTankStartMinMax height;
	LandscapeDefnTankStartPositionList positions;
};

#endif
