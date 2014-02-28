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

#include <tank/TankModelStore.h>
#include <common/Defines.h>
#include <lang/LangResource.h>

TankModelStore::TankModelStore() :
	XMLEntryRoot<XMLEntryContainer>(S3D::eModLocation, "data/tanks.xml", "tanks",
		"TankModelStore", "The set of tank model definitions that can be used by players")

{
	modelCatagories_.insert("All");
	addChildXMLEntry("tank", &models_);
}

TankModelStore::~TankModelStore()
{
	clear();
}

bool TankModelStore::loadTankMeshes(ScorchedContext &context, ProgressCounter *counter)
{
	clear();

	if (counter) counter->setNewOp(LANG_RESOURCE("LOADING_TANKS", "Loading tanks"));

	// Load tank definition file
	if (!loadFile(true)) return false;

	std::list<TankModel *>::iterator itor = models_.getChildren().begin(),
		end = models_.getChildren().end();
	for (;itor!=end;++itor)
	{
		// Add catagories to list of all catagories
		TankModel *tankModel = *itor;
		std::list<XMLEntryString *> &catagories = tankModel->getCatagories();
		std::list<XMLEntryString *>::iterator catitor = catagories.begin(),
			catend = catagories.end();
		for (;catitor!=catend;++catitor)
		{
			modelCatagories_.insert((*catitor)->getValue());
		}
	}

	return true;
}

TankModel *TankModelStore::getRandomModel(int team, bool ai, const char *tankType)
{
	std::vector<TankModel *> models;
	std::list<TankModel *>::iterator itor = models_.getChildren().begin(),
		end = models_.getChildren().end();
	for (;itor!=end;++itor)
	{
		TankModel *model = (*itor);
		if (strcmp(model->getName(), "Random") != 0)
		{
			if (model->isOfTeam(team) && 
				model->isOfAi(ai) &&
				model->isOfTankType(tankType)) 
			{
				models.push_back(model);
			}
		}
	}

	if (models.empty()) 
	{
		S3D::dialogExit("TankModelStore",
			S3D::formatStringBuffer("Failed to find any tank models that match the "
				"current team (%i), ai (%s) and tanktype (%s)",
				team, ai?"yes":"no", tankType));
	}
	unsigned int randomNumber = rand();
	TankModel *model = models[randomNumber % models.size()];
	return model;
}

TankModel *TankModelStore::getModelByName(const char *name)
{
	DIALOG_ASSERT(models_.getChildren().size());

	if (strcmp(name, "Random") == 0) return 0;

	std::list<TankModel *>::iterator itor = models_.getChildren().begin(),
		end = models_.getChildren().end();
	for (;itor!=end;++itor)
	{
		TankModel *current = (*itor);
		if (0 == strcmp(current->getName(), name))
		{
			return current;
		}
	}

	return 0;
}

void TankModelStore::clear() 
{
	models_.clear();
	modelCatagories_.clear();
}
