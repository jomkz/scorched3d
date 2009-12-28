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

void TankModelContainer::setServerTankModelName(const char *serverModelName)
{
	serverModelName_ = serverModelName;
	setTankModel();
}

void TankModelContainer::setCustomTankModelName(const char *customModelName)
{
	customModelName_ = customModelName;
	setTankModel();
}

void TankModelContainer::setTankModel()
{
#ifndef S3D_SERVER
	if (tank_->getRenderer())
	{
		TargetRendererImplTank *renderer = (TargetRendererImplTank *)
			tank_->getRenderer();
		renderer->resetModel();
	}
#endif

	tankModel_ = context_.getTankModels().getModelByName(customModelName_.c_str());
	if (!tankModel_ || !tankModel_->availableForTank(tank_))
	{
		tankModel_ = context_.getTankModels().getModelByName(serverModelName_.c_str());
		DIALOG_ASSERT(tankModel_);
	}
}

bool TankModelContainer::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(customModelName_);
	buffer.addToBuffer(serverModelName_);
	return true;
}

bool TankModelContainer::readMessage(NetBufferReader &reader)
{
	std::string customModelName, serverModelName;
	if (!reader.getFromBuffer(customModelName)) return false;
	if (!reader.getFromBuffer(serverModelName)) return false;
	setCustomTankModelName(customModelName.c_str());
	setServerTankModelName(serverModelName.c_str());
	return true;
}

void TankModelContainer::toString(std::string &str)
{
	str.append("  ModelContainer:\n");
	str.append("    Custom : ").append(customModelName_).append("\n");
	str.append("    Server : ").append(serverModelName_).append("\n");
}
