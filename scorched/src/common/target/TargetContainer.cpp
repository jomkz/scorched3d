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

#include <target/TargetContainer.h>

TargetContainer::TargetContainer()
{
}

TargetContainer::~TargetContainer()
{
}

void TargetContainer::addTarget(Target *target)
{
	DIALOG_ASSERT(target->isTarget());
	internalAddTarget(target);
}

void TargetContainer::internalAddTarget(Target *target)
{
	std::map<unsigned int, Target *>::iterator findItor = 
		targets_.find(target->getPlayerId());
	if (findItor != targets_.end())
	{
		Target *original = (*findItor).second;
		S3D::dialogExit("Scorched3D",
			S3D::formatStringBuffer("Duplicate target %u being added to container.\n"
			"Original :%s, this %s",
			target->getPlayerId(),
			original->getCStrName().c_str(),
			target->getCStrName().c_str()));
	}

	targets_[target->getPlayerId()] = target;
}

Target *TargetContainer::removeTarget(unsigned int playerId)
{
	Target *target = internalRemoveTarget(playerId);
	DIALOG_ASSERT(target->isTarget());
	return target;
}

Target *TargetContainer::internalRemoveTarget(unsigned int playerId)
{
    std::map<unsigned int, Target *>::iterator itor =
		targets_.find(playerId);
	if (itor != targets_.end())
	{
		Target *current = (*itor).second;
		targets_.erase(itor);
		return current;		
	}
	return 0;
}

Target *TargetContainer::getTargetById(unsigned int id)
{
	std::map<unsigned int, Target *>::iterator mainitor =
		targets_.find(id);
	if (mainitor != targets_.end())
	{
		Target *target = (*mainitor).second;
		DIALOG_ASSERT(target->getPlayerId() == id);

		return target;
	}
	return 0;
}
