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

#include <landscapedef/LandscapeMusic.h>
#include <math.h>

static XMLEntryEnum::EnumEntry LandscapeMusicStateListEnum[] =
{
	{ "loading", LandscapeMusicStateList::StateLoading },
	{ "buying", LandscapeMusicStateList::StateBuying },
	{ "playing", LandscapeMusicStateList::StatePlaying },
	{ "shot", LandscapeMusicStateList::StateShot },
	{ "score", LandscapeMusicStateList::StateScore },
	{ "wait", LandscapeMusicStateList::StateWait },
	{ "", -1 }
};

LandscapeMusicStateList::LandscapeMusicStateList() :
	XMLEntryList<XMLEntryEnum>("The list of state names that this music should be played in.")
{
}

LandscapeMusicStateList::~LandscapeMusicStateList()
{
}

XMLEntryEnum *LandscapeMusicStateList::createXMLEntry(void *xmlData)
{
	return new XMLEntryEnum("Play/enable the given music for the given state.", LandscapeMusicStateListEnum);
}

LandscapeMusic::LandscapeMusic() :
	XMLEntryContainer("LandscapeMusic", "Defines a music file that will be played continuously in the given states."),
	file("The location of the music file to play"),
	gain("The gain (volume) of the played music", 0, 1),
	playstatelist()
{
	addChildXMLEntry("file", &file, "gain", &gain, "playstate", &playstatelist);
}

LandscapeMusic::~LandscapeMusic()
{
}

LandscapeMusicList::LandscapeMusicList() : 
	XMLEntryList<LandscapeMusic>("Defines a music file that will be played continuously in the given states.")
{
}

LandscapeMusicList::~LandscapeMusicList()
{
}

LandscapeMusic *LandscapeMusicList::createXMLEntry(void *xmlData)
{
	return new LandscapeMusic();
}
