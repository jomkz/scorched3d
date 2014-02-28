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

#if !defined(__INCLUDE_LandscapeMusich_INCLUDE__)
#define __INCLUDE_LandscapeMusich_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class LandscapeMusicStateList : public XMLEntryList<XMLEntryEnum>
{
public:
	enum PlayState
	{
		StateNone,
		StateWait,
		StateLoading,
		StateBuying,
		StatePlaying,
		StateShot,
		StateScore
	};

	LandscapeMusicStateList();
	virtual ~LandscapeMusicStateList();

	virtual XMLEntryEnum *createXMLEntry();
};

class LandscapeMusic : public XMLEntryContainer
{
public:
	LandscapeMusic();
	virtual ~LandscapeMusic();

	XMLEntryFile file;
	XMLEntryFixed gain;
	LandscapeMusicStateList playstatelist;
};

class LandscapeMusicList : public XMLEntryList<LandscapeMusic>
{
public:
	LandscapeMusicList();
	virtual ~LandscapeMusicList();

	virtual LandscapeMusic *createXMLEntry();
};

#endif // __INCLUDE_LandscapeMusich_INCLUDE__
