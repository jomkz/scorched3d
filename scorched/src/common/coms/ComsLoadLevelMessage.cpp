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

#include <landscapedef/LandscapeDefinition.h>
#include <coms/ComsLoadLevelMessage.h>
#include <server/ScorchedServer.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <weapons/AccessoryStore.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#endif

ComsMessageType ComsLoadLevelMessage::ComsLoadLevelMessageType("ComsLoadLevelMessageType");

ComsLoadLevelMessage::ComsLoadLevelMessage(LandscapeDefinition *landscapeDefintion) :
	ComsMessage(ComsLoadLevelMessageType)
{
	if (landscapeDefintion) landscapeDefinition_ = *landscapeDefintion;
}

ComsLoadLevelMessage::~ComsLoadLevelMessage()
{
}

bool ComsLoadLevelMessage::writeMessage(NetBuffer &buffer)
{
	if (!landscapeDefinition_.writeMessage(buffer)) return false;
	if (!ScorchedServer::instance()->
		getOptionsGame().getMainOptions().writeToBuffer(buffer, false, false)) return false;
	if (!ScorchedServer::instance()->
		getOptionsTransient().writeToBuffer(buffer)) return false;
	if (!ScorchedServer::instance()->getAccessoryStore().
		writeEconomyToBuffer(buffer)) return false;

	return true;
}

bool ComsLoadLevelMessage::readMessage(NetBufferReader &reader)
{
#ifndef S3D_SERVER
	if (!landscapeDefinition_.readMessage(reader)) return false;
	if (!ScorchedClient::instance()->
		getOptionsGame().getMainOptions().readFromBuffer(reader, false, false)) return false;
	ScorchedClient::instance()->getOptionsGame().updateLevelOptions(
		ScorchedClient::instance()->getContext(), landscapeDefinition_);
	if (!ScorchedClient::instance()->
		getOptionsTransient().readFromBuffer(reader)) return false;
	if (!ScorchedClient::instance()->getAccessoryStore().
		readEconomyFromBuffer(reader)) return false;
#endif

	return true;
}
