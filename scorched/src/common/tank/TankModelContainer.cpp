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
#ifndef S3D_SERVER
	#include <tankgraph/TargetRendererImplTank.h>
#endif
#include <engine/ScorchedContext.h>
#include <common/DefinesString.h>

TankModelContainer::TankModelContainer(
	const char *modelName, 
	const char *typeName) :
	tankModelName_(modelName), 
	tankTypeName_(typeName),
	tankOriginalModelName_(modelName)
{
}

TankModelContainer::~TankModelContainer()
{
}

void TankModelContainer::setTankModelName(
	const char *modelName, const char *originalModelName, const char *typeName)
{
	if (0 != strcmp(modelName, tankModelName_.c_str()) ||
		0 != strcmp(originalModelName, tankOriginalModelName_.c_str()))
	{
#ifndef S3D_SERVER
		if (tank_->getRenderer())
		{
			TargetRendererImplTank *renderer = (TargetRendererImplTank *)
				tank_->getRenderer();
			renderer->resetModel();
		}
#endif
	}

	tankModelName_ = modelName;
	tankOriginalModelName_ = originalModelName;
	tankTypeName_ = typeName;
}

bool TankModelContainer::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(tankModelName_);
	buffer.addToBuffer(tankOriginalModelName_);
	buffer.addToBuffer(tankTypeName_);
	return true;
}

bool TankModelContainer::readMessage(NetBufferReader &reader)
{
	std::string newModelName, newTypeName, newOrignalName;
	if (!reader.getFromBuffer(newModelName)) return false;
	if (!reader.getFromBuffer(newOrignalName)) return false;
	if (!reader.getFromBuffer(newTypeName)) return false;
	setTankModelName(
		newModelName.c_str(), 
		newOrignalName.c_str(), 
		newTypeName.c_str());
	return true;
}
