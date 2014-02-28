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

#include <tank/TankModel.h>
#include <tank/Tank.h>
#include <tanket/TanketType.h>
#include <XML/XMLNode.h>
#include <engine/ScorchedContext.h>

TankModelTypeList::TankModelTypeList() :
	XMLEntryList<XMLEntryString>("The set of tank types that this model can be used by", 0)
{
}

TankModelTypeList::~TankModelTypeList()
{
}

XMLEntryString *TankModelTypeList::createXMLEntry(void *xmlData)
{
	return new XMLEntryString("A tank type that this model belongs to, "
		"only tanks of this type will be able to select this model.  "
		"Models with no defined tank types are available for all types.");
}

TankModelCatagoryList::TankModelCatagoryList() :
	XMLEntryList<XMLEntryString>("The set of tank catagories that this model belongs to", 0)
{
}

TankModelCatagoryList::~TankModelCatagoryList()
{
}

XMLEntryString *TankModelCatagoryList::createXMLEntry(void *xmlData)
{
	return new XMLEntryString("A visual catagory that this model belongs to, "
		"used for display purposes to allow the user to easily select models.");
}

TankModelTeamList::TankModelTeamList() :
	XMLEntryList<XMLEntryInt>("The set of teams that this model can be used by", 0)
{
}

TankModelTeamList::~TankModelTeamList()
{
}

XMLEntryInt *TankModelTeamList::createXMLEntry(void *xmlData)
{
	return new XMLEntryInt("A team type that this model belongs to, "
		"only tanks of this team will be able to select this model."
		"Models with no defined teams are available for all teams.");
}

TankModel::TankModel() :
	XMLEntryContainer("TankModel",
		"The definition of a tank model that can be used by players (both human and computer controlled)."),
	aiOnly_("This tank is only available to computer controlled players (bots)", 0, false),
	modelName_("The name of the model")
{
	catagories_.getChildren().push_back(new XMLEntryString("", 0, "ALL"));
	addChildXMLEntry("name", &modelName_);
	addChildXMLEntry("model", &modelId_);
	addChildXMLEntry("type", &tankTypes_);
	addChildXMLEntry("catagory", &catagories_);
	addChildXMLEntry("team", &teams_);
}

TankModel::~TankModel()
{
}

bool TankModel::isOfCatagory(const char *catagory)
{
	std::list<XMLEntryString *>::iterator itor = catagories_.getChildren().begin(),
		end = catagories_.getChildren().end();
	for (;itor!=end;++itor)
	{
		if ((*itor)->getValue() == catagory) return true;
	}
	return false;
}

bool TankModel::isOfTankType(const char *tankType)
{
	if (tankTypes_.getChildren().empty()) return true;
	std::list<XMLEntryString *>::iterator itor = tankTypes_.getChildren().begin(),
		end = tankTypes_.getChildren().end();
	for (;itor!=end;++itor)
	{
		if ((*itor)->getValue() == tankType) return true;
	}
	return false;
}

bool TankModel::isOfAi(bool ai)
{
	if (!aiOnly_.getValue()) return true;
	if (ai) return true;
	return false;
}

bool TankModel::isOfTeam(int team)
{
	if (team == 0) return true; // No Team

	if (teams_.getChildren().empty()) return true;
	std::list<XMLEntryInt *>::iterator itor = teams_.getChildren().begin(),
		end = teams_.getChildren().end();
	for (;itor!=end;++itor)
	{
		if ((*itor)->getValue() == team) return true;
	}
	return false;
}

bool TankModel::availableForTank(Tank *tank)
{
	if (isOfTeam(tank->getTeam()) &&
		isOfAi(tank->getDestinationId() == 0) &&
		isOfTankType(tank->getTanketType()->getName()))
	{
		return true;
	}

	return false;
}

TankModelList::TankModelList() :
	XMLEntryList<TankModel>("The list of tank models that can be used by players", 0)
{
}

TankModelList::~TankModelList()
{
}

TankModel *TankModelList::createXMLEntry(void *xmlData)
{
	return new TankModel();
}
