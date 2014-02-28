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

#include <placement/PlacementType.h>
#include <landscapedef/LandscapeSound.h>
#include <landscapedef/LandscapeInclude.h>
#include <landscapedef/LandscapeEvent.h>
#include <landscapedef/LandscapeMovement.h>
#include <landscapedef/LandscapeSound.h>
#include <landscapedef/LandscapeMusic.h>
#include <landscapedef/LandscapeOptions.h>

LandscapeInclude::LandscapeInclude()
{
	events = new LandscapeEventList();
	movements = new LandscapeMovementTypeList();
	sounds = new LandscapeSoundList();
}

LandscapeInclude::~LandscapeInclude()
{
	delete events;
	delete movements;
	delete sounds;
	{
		while (!placements.empty())
		{
			delete placements.back();
			placements.pop_back();
		}
	}
	{
		while (!musics.empty())
		{
			delete musics.back();
			musics.pop_back();
		}
	}
	{
		while (!options.empty())
		{
			delete options.back();
			options.pop_back();
		}
	}
}

bool LandscapeInclude::readXML(LandscapeDefinitions *definitions, XMLNode *node)
{
	if (!events->readXML(node)) return false;
	if (!movements->readXML(node)) return false;
	if (!sounds->readXML(node)) return false;
	{
		XMLNode *musicNode;
		while (node->getNamedChild("music", musicNode, false))
		{
			LandscapeMusicType *music = new LandscapeMusicType;
			if (!music->readXML(musicNode)) return false;
			musics.push_back(music);
		}
	}
	{
		XMLNode *optionsNode;
		while (node->getNamedChild("options", optionsNode, false))
		{
			LandscapeOptionsType *option = new LandscapeOptionsType;
			if (!option->readXML(optionsNode)) return false;
			options.push_back(option);
		}
	}
	{
		XMLNode *placementNode;
		while (node->getNamedChild("placement", placementNode, false))
		{
			std::string placementtype;
			PlacementType *placement = 0;
			if (!placementNode->getNamedParameter("type", placementtype)) return false;
			if (!(placement = PlacementType::create(placementtype.c_str()))) return false;
			if (!placement->readXML(placementNode)) return false;
			placements.push_back(placement);
		}
	}
	return node->failChildren();
}
