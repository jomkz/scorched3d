////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <landscapedef/LandscapeDefinition.h>
#include <coms/ComsLevelMessage.h>
#include <common/Defines.h>

ComsLevelMessage::ComsLevelMessage() :
	ComsMessage("ComsLevelMessage")
{

}

ComsLevelMessage::~ComsLevelMessage()
{
}

void ComsLevelMessage::createMessage(LandscapeDefinition &hdef)
{
	hdef_ = hdef;
}

LandscapeDefinition &ComsLevelMessage::getGroundMapsDefn()
{ 
	return hdef_; 
}

bool ComsLevelMessage::writeMessage(NetBuffer &buffer)
{
	if (!hdef_.writeMessage(buffer)) return false;
	if (!hMap_.writeMessage(buffer)) return false;
	buffer.addToBuffer(newTargets_);
	buffer.addToBuffer(oldTargets_);

	buffer.addToBuffer((int) tankPositions_.size());
	{
		std::list<FixedVector>::iterator itor;
		for (itor = tankPositions_.begin();
			itor != tankPositions_.end();
			itor++)
		{
			buffer.addToBuffer(*itor);
		}
	}

	buffer.addToBuffer((int) targetIds_.size());
	{
		std::set<unsigned int>::iterator itor;
		for (itor = targetIds_.begin();
			itor != targetIds_.end();
			itor++)
		{
			buffer.addToBuffer(*itor);
		}
	}

	return true;
}

bool ComsLevelMessage::readMessage(NetBufferReader &reader)
{
	if (!hdef_.readMessage(reader)) return false;
	if (!hMap_.readMessage(reader)) return false;
	if (!reader.getFromBuffer(newTargets_)) return false;
	if (!reader.getFromBuffer(oldTargets_)) return false;

	int tankPosSize = 0;
	if (!reader.getFromBuffer(tankPosSize)) return false;
	for (int i=0; i<tankPosSize; i++)
	{
		FixedVector tankPos;
		if (!reader.getFromBuffer(tankPos)) return false;
		tankPositions_.push_back(tankPos);
	}

	int targetIdSize = 0;
	if (!reader.getFromBuffer(targetIdSize)) return false;
	for (int i=0; i<targetIdSize; i++)
	{
		unsigned int targetId = 0;
		if (!reader.getFromBuffer(targetId)) return false;
		targetIds_.insert(targetId);
	}

	return true;
}

