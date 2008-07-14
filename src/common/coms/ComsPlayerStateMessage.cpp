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

#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#endif
#include <server/ScorchedServer.h>
#include <coms/ComsPlayerStateMessage.h>
#include <tank/TankTeamScore.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <target/TargetState.h>
#include <movement/TargetMovement.h>
#include <tankai/TankAIAdder.h>
#include <common/Logger.h>
#include <set>

ComsPlayerStateMessage::ComsPlayerStateMessage(bool targets, bool accessories) : 
	ComsMessage("ComsPlayerStateMessage"),
	targets_(targets), accessories_(accessories)
{

}

ComsPlayerStateMessage::~ComsPlayerStateMessage()
{

}

bool ComsPlayerStateMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(targets_);

	// Team score
	if (!ScorchedServer::instance()->getContext().getTankTeamScore().
		writeMessage(buffer)) return false;

	// Tanks
	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	buffer.addToBuffer((int) tanks.size());
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		// Add each tank
		Tank *tank = (*itor).second;
		buffer.addToBuffer(tank->getPlayerId());
		if (!tank->writeMessage(buffer, accessories_)) return false;
	}

	if (targets_)
	{
		// Target movement
		if (!ScorchedServer::instance()->getTargetMovement().writeMessage(buffer)) return false;

		// Get list of targets to send
		std::map<unsigned int, Target *> targets;
		{
			std::map<unsigned int, Target *> &possibletargets =
				ScorchedServer::instance()->getTargetContainer().getTargets();
			std::map<unsigned int, Target *>::iterator itor;
			for (itor = possibletargets.begin();
				itor != possibletargets.end();
				itor++)
			{
				Target *target = (*itor).second;
				if (target->isTarget())
				{
					// send the state of targets than can move
					if (target->getTargetState().getMovement())
					{
						targets[(*itor).first] = (*itor).second;
					}
				}
			}
		}

		// Send targets
		buffer.addToBuffer((int) targets.size());
		std::map<unsigned int, Target *>::iterator itor;
		for (itor = targets.begin();
			itor != targets.end();
			itor++)
		{
			// Add each tank
			Target *target = (*itor).second;
			buffer.addToBuffer(target->getPlayerId());

			if (!target->writeMessage(buffer)) return false;
		}
	}

	return true;
}

bool ComsPlayerStateMessage::readMessage(NetBufferReader &reader)
{
#ifndef S3D_SERVER
	if (!reader.getFromBuffer(targets_)) return false;

	if (!ScorchedClient::instance()->getContext().getTankTeamScore().
		readMessage(reader)) return false;

	// Update all targets with the state from the targets on the
	// server
	int count = 0;
	if (!reader.getFromBuffer(count)) return false;
	for (int i=0; i<count; i++)
	{
		unsigned int playerId;
		if (!reader.getFromBuffer(playerId)) return false;
		Tank *tank = ScorchedClient::instance()->getTankContainer().
			getTankById(playerId);
		if (tank)
		{
			if (!tank->readMessage(reader)) return false;
		}
		else
		{
			std::string name;
			reader.getFromBuffer(name);
			Logger::log(S3D::formatStringBuffer("Error: Failed to find tank %u\"%s\"",
				playerId, name.c_str()));

			std::map<unsigned int, Tank *> &tanks = 
				ScorchedClient::instance()->getTankContainer().getAllTanks();
			std::map<unsigned int, Tank *>::iterator itor;
			for (itor = tanks.begin();
				itor != tanks.end();
				itor++)
			{
				Tank *tank = itor->second;
				Logger::log(S3D::formatStringBuffer("  Possible tank %u\"%s\"",
					tank->getPlayerId(), tank->getName()));
			}

			return false;
		}
	}

	if (targets_)
	{
		// Get all of the movement information
		if (!ScorchedClient::instance()->getTargetMovement().readMessage(reader)) return false;

		int count = 0;
		if (!reader.getFromBuffer(count)) return false;
		for (int i=0; i<count; i++)
		{
			unsigned int playerId;
			if (!reader.getFromBuffer(playerId)) return false;
			Target *target = ScorchedClient::instance()->getTargetContainer().
				getTargetById(playerId);
			if (target)
			{
				if (!target->readMessage(reader)) return false;
			}
			else
			{
				std::string name;
				reader.getFromBuffer(name);
				Logger::log(S3D::formatStringBuffer("Error: Failed to find target %u\"%s\"",
					playerId, name.c_str()));
				return false;
			}
		}
	}
#endif
	
	return true;
}
