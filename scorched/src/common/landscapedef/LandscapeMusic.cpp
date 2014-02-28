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

LandscapeMusicStateList::LandscapeMusicStateList() :
	XMLEntryList<XMLEntryString>("LandscapeMusicStateList", 
		"The list of state names that this music should be played in.")
{
}

LandscapeMusicStateList::~LandscapeMusicStateList()
{
}

XMLEntryString *LandscapeMusicStateList::createXMLEntry()
{
	return new XMLEntryString("Play/enable the given music for the given state.");
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

bool LandscapeMusic::readXML(XMLNode *node)
{
	if (!XMLEntryContainer::readXML(node)) return false;

	std::list<XMLEntryString *>::iterator itor = playstatelist.getChildren().begin(),
		end = playstatelist.getChildren().end();

	for (;itor!=end; itor++)
	{
		std::string state = (*itor)->getValue();
		PlayState playstate;
		if (0 == strcmp(state.c_str(), "loading")) playstate = StateLoading;
		else if (0 == strcmp(state.c_str(), "buying")) playstate = StateBuying;
		else if (0 == strcmp(state.c_str(), "playing")) playstate = StatePlaying;
		else if (0 == strcmp(state.c_str(), "shot")) playstate = StateShot;
		else if (0 == strcmp(state.c_str(), "score")) playstate = StateScore;
		else if (0 == strcmp(state.c_str(), "wait")) playstate = StateWait;
		else return node->returnError("Unknown playstate type");
		playstates.push_back(playstate);
	}
	if (playstates.empty()) return node->returnError("No playstats defined");
	return true;
}

LandscapeMusicList::LandscapeMusicList() : 
	XMLEntryList<LandscapeMusic>("LandscapeMusicList", "Defines a music file that will be played continuously in the given states.")
{
}

LandscapeMusicList::~LandscapeMusicList()
{
}

LandscapeMusic *LandscapeMusicList::createXMLEntry()
{
	return new LandscapeMusic();
}
