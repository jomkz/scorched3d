////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <tank/TankModelContainer.h>
#include <tank/TankModelStore.h>
#include <tank/Tank.h>
#ifndef S3D_SERVER
	#include <tankgraph/TargetRendererImplTank.h>
#endif
#include <engine/ScorchedContext.h>
#include <common/DefinesString.h>

TankModelContainer::TankModelContainer(ScorchedContext &context) :
	context_(context),
	tankModel_(0), tank_(0)
{
}

TankModelContainer::~TankModelContainer()
{
}

TankModel *TankModelContainer::getTankModel() 
{ 
	if (!tankModel_)
	{
	#ifndef S3D_SERVER
		if (tank_->getRenderer())
		{
			TargetRendererImplTank *renderer = (TargetRendererImplTank *)
				tank_->getRenderer();
			renderer->resetModel();
		}
	#endif

		tankModel_ = context_.getTankModels().getModelByName(modelName_.c_str());
		if (!tankModel_)
		{
			tankModel_ = context_.getTankModels().getRandomModel(
				tank_->getTeam(), tank_->getDestinationId() == 0);
		}
	}

	return tankModel_; 
}

void TankModelContainer::setTankModelName(const char *modelName)
{
	modelName_ = modelName;
	tankModel_ = 0;
}

bool TankModelContainer::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TankModelContainer");

	buffer.addToBufferNamed("modelName", modelName_);
	return true;
}

bool TankModelContainer::readMessage(NetBufferReader &reader)
{
	std::string modelName;
	if (!reader.getFromBuffer(modelName)) return false;
	setTankModelName(modelName.c_str());
	return true;
}
