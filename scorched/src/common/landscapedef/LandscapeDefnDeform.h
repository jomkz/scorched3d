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

#if !defined(__INCLUDE_LandscapeDefnDeformh_INCLUDE__)
#define __INCLUDE_LandscapeDefnDeformh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class LandscapeDefnDeform : public XMLEntryContainer
{
public:
	enum DefnDeformType
	{
		eDeformFile, 
		eDeformSolid,
		eDeformDeform
	};

	LandscapeDefnDeform(const char *name, const char *description);
	virtual ~LandscapeDefnDeform();

	virtual DefnDeformType getType() = 0;
};

class LandscapeDefnDeformChoice : public XMLEntryTypeChoice<LandscapeDefnDeform>
{
public:
	LandscapeDefnDeformChoice();
	virtual ~LandscapeDefnDeformChoice();

	virtual LandscapeDefnDeform *createXMLEntry(const std::string &type);
};

class LandscapeDefnDeformFile : public LandscapeDefnDeform
{
public:
	LandscapeDefnDeformFile();
	virtual ~LandscapeDefnDeformFile();

	XMLEntryFile file;
	XMLEntryBool levelsurround;

	virtual DefnDeformType getType() { return eDeformFile; }
};

class LandscapeDefnDeformSolid : public LandscapeDefnDeform
{
public:
	LandscapeDefnDeformSolid();
	virtual ~LandscapeDefnDeformSolid();

	virtual DefnDeformType getType() { return eDeformSolid; }
};

class LandscapeDefnDeformDeform : public LandscapeDefnDeform
{
public:
	LandscapeDefnDeformDeform();
	virtual ~LandscapeDefnDeformDeform();

	virtual DefnDeformType getType() { return eDeformDeform; }
};

#endif
