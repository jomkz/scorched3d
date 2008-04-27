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
	buffer.addToBuffer(newTargets_);
	buffer.addToBuffer(oldTargets_);

	buffer.addToBuffer((int) deformInfos_.size());
	{
		std::vector<DeformLandscape::DeformInfo>::iterator itor;
		for (itor = deformInfos_.begin();
			itor != deformInfos_.end();
			itor++)
		{
			DeformLandscape::DeformInfo &info = *itor;
			buffer.addToBuffer(info.type);
			buffer.addToBuffer(info.pos);
			buffer.addToBuffer(info.radius);
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
	if (!reader.getFromBuffer(newTargets_)) return false;
	if (!reader.getFromBuffer(oldTargets_)) return false;

	int infosSize = 0;
	if (!reader.getFromBuffer(infosSize)) return false;
	for (int i=0; i<infosSize; i++)
	{
		DeformLandscape::DeformInfo info;
		if (!reader.getFromBuffer(info.type)) return false;
		if (!reader.getFromBuffer(info.pos)) return false;
		if (!reader.getFromBuffer(info.radius)) return false;
		deformInfos_.push_back(info);
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

