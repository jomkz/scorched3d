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

#include <common/Defines.h>
#include <tankai/TankAIStore.h>
#include <tankai/TankAICurrent.h>
#include <tankai/TankAIShallow.h>
#include <tankai/TankAIRandom.h>
#include <stdlib.h>

TankAIStore::TankAIStore()
{

}

TankAIStore::~TankAIStore()
{
	clearAIs();
}

void TankAIStore::clearAIs()
{
	while (!ais_.empty())
	{
		TankAI *ai = ais_.front();
		ais_.pop_front();
		delete ai;
	}
}

bool TankAIStore::loadAIs(bool shallow)
{
	if (!shallow)
	{
		tankAiWeaponSets_.parseConfig();
	}

	if (shallow)
	{
		if (!shallowFile_.loadFile(true, &tankAiWeaponSets_)) return false;
		std::list<TankAIShallow *>::iterator itor = shallowFile_.tankAis_.getChildren().begin(),
			end = shallowFile_.tankAis_.getChildren().end();
		for (;itor!=end;++itor)
		{
			addAI(*itor);
		}
	}
	else
	{
		if (!currentFile_.loadFile(true, &tankAiWeaponSets_)) return false;
		std::list<TankAICurrent *>::iterator itor = currentFile_.tankAis_.getChildren().begin(),
			end = currentFile_.tankAis_.getChildren().end();
		for (;itor!=end;++itor)
		{
			addAI(*itor);
		}
	}

	// Add the random player
	{
		TankAIRandom *tankAI = new TankAIRandom;
		bool foundRandom = false;
		std::list<TankAI *>::iterator itor;
		for (itor = ais_.begin();
			itor != ais_.end();
			++itor)
		{
			TankAI *ai = (*itor);
			if (ai->availableForRandom())
			{
				foundRandom = true;
				tankAI->addTankAI(ai);
			}
		}
		if (!foundRandom)
		{
			S3D::dialogMessage("TankAIStore",
						  "No tank ais are marked availableforrandom");
			return false;
		}

		addAI(tankAI);
	}

	return true;
}

TankAI *TankAIStore::getAIByName(const char *name)
{
	// Computers
	std::list<TankAI *>::iterator itor;
	for (itor = ais_.begin();
		itor != ais_.end();
		++itor)
	{
		if (!strcmp((*itor)->getName(), name)) return (*itor);
	}

	return 0;
}

void TankAIStore::addAI(TankAI *ai)
{
	ais_.push_back(ai);
}
