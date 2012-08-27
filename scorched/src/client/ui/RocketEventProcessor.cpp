////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <ui/RocketEventProcessor.h>
#include <common/DefinesAssert.h>
#include <common/DefinesString.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <coms/ComsTankChangeMessage.h>
#include <coms/ComsMessageSender.h>
#include <stdio.h>

RocketEventProcessor &RocketEventProcessor::instance()
{
	static RocketEventProcessor instance;
	return instance;
}

RocketEventProcessor::RocketEventProcessor()
{
}

RocketEventProcessor::~RocketEventProcessor()
{
}

void RocketEventProcessor::ProcessEvent(Rocket::Core::String &eventString, Rocket::Core::Event &evt)
{
	size_t lbracketPos = eventString.Find("(");
	if (lbracketPos != Rocket::Core::String::npos)
	{
		size_t rbracketPos = eventString.Find(")", lbracketPos);
		if (rbracketPos != Rocket::Core::String::npos)
		{
			Rocket::Core::String actionString = eventString.Substring(0, lbracketPos);
			size_t current = lbracketPos + 1, next;
			std::list<Rocket::Core::String> params;
			while ((next = eventString.Find(",", current)) != Rocket::Core::String::npos)
			{
				params.push_back(eventString.Substring(current, next - current));
				current = next + 1;
			}
			params.push_back(eventString.Substring(current, rbracketPos - current));
			ProcessAction(actionString, params);
		}
	}
}

void RocketEventProcessor::ProcessAction(Rocket::Core::String &action, std::list<Rocket::Core::String> &params)
{
	if (action == "stimulus")
	{
		if (params.size() !=1)
		{
			S3D::dialogExit("RocketEventProcessor", 
				S3D::formatStringBuffer("Action %s requires the state parameter", 
				action.CString()));
		}
		else
		{
			ScorchedClient::instance()->getClientState().setStateString(params.front().CString());
		}
	}
	else if (action == "joingame")
	{
		unsigned int current = 0, currentPlayerId_ = 0;
		std::map<unsigned int, Tank *> &tanks = 
			ScorchedClient::instance()->getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			++itor)
		{
			Tank *tank = (*itor).second;
			if ((tank->getDestinationId() == 
				ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId()) &&
				(tank->getPlayerId() != TargetID::SPEC_TANK_ID))
			{
				if (current == 0)
				{
					currentPlayerId_ = tank->getPlayerId();
					break;
				}
				else if (tank->getPlayerId() == current) 
				{
					current = 0;
				}
			}
		}

		// Add this player
		ComsTankChangeMessage message(currentPlayerId_,
			LANG_STRING("TestName"),
			Vector(),
			"none",
			"none",
			ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId(),
			0,
			"Human",
			false);
		// Add avatar (if not one)
		// TODO
		/*
		Tank *tank = ScorchedClient::instance()->getTargetContainer().
			getTankById(currentPlayerId_);
		if (tank && 
			strcmp(tank->getAvatar().getName(), imageList_->getCurrentShortPath()) != 0)
		{
			if (tank->getAvatar().loadFromFile(imageList_->getCurrentLongPath()))
			{
				if (tank->getAvatar().getFile().getBufferUsed() <=
					(unsigned) ScorchedClient::instance()->getOptionsGame().getMaxAvatarSize())
				{
					message.setPlayerIconName(imageList_->getCurrentShortPath());
					message.getPlayerIcon().addDataToBuffer(
						tank->getAvatar().getFile().getBuffer(),
						tank->getAvatar().getFile().getBufferUsed());
				}
				else
				{
					Logger::log( "Warning: Avatar too large to send to server");
				}
			}
		}
		*/
		ComsMessageSender::sendToServer(message);
	}
	else
	{
		S3D::dialogExit("RocketEventProcessor", 
			S3D::formatStringBuffer("Failed to action %s", 
			action.CString()));
	}
}
