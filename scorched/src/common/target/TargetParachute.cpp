////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <target/TargetParachute.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>

TargetParachute::TargetParachute(ScorchedContext &context) : 
	context_(context),
	currentParachute_(0)
{
}

TargetParachute::~TargetParachute()
{
}

void TargetParachute::newGame()
{
	setCurrentParachute(0);
}

void TargetParachute::setCurrentParachute(Accessory *para)
{
	currentParachute_ = para;
}

bool TargetParachute::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((unsigned int)(currentParachute_?currentParachute_->getAccessoryId():0));
	return true;
}

bool TargetParachute::readMessage(NetBufferReader &reader)
{
	unsigned int paraId;
	if (!reader.getFromBuffer(paraId)) return false;
	if (paraId == 0) setCurrentParachute(0);
	else setCurrentParachute(context_.getAccessoryStore().findByAccessoryId(paraId));
	return true;
}
