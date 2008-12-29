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

#include <coms/ComsSyncCheckMessage.h>
#include <common/Logger.h>
#include <common/OptionsTransient.h>
#include <engine/ActionController.h>
#ifndef S3D_SERVER
	#include <client/ScorchedClient.h>
#endif
#include <server/ScorchedServer.h>
#include <target/TargetContainer.h>
#include <target/TargetState.h>
#include <target/TargetLife.h>
#include <tank/TankState.h>
#include <tank/Tank.h>
#include <landscapemap/LandscapeMaps.h>
#include <set>

ComsSyncCheckMessage::ComsSyncCheckMessage() :
	ComsMessage("ComsSyncCheckMessage")
{

}

ComsSyncCheckMessage::~ComsSyncCheckMessage()
{

}

bool ComsSyncCheckMessage::writeMessage(NetBuffer &buffer)
{
	// Send action sync data
	std::vector<std::string>::iterator syncItor;
	std::vector<std::string> &syncs = 
		ScorchedServer::instance()->getActionController().getSyncCheck();
	buffer.addToBuffer((int) syncs.size());
	for (syncItor = syncs.begin();
		syncItor != syncs.end();
		syncItor++)
	{
		buffer.addToBuffer(syncItor->c_str());
	}

	// Send the height map data
	HeightMap &map = ScorchedServer::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap();
	for (int y=0; y<map.getMapHeight(); y++)
	{
		for (int x=0; x<map.getMapWidth(); x++)
		{
			fixed height = map.getHeight(x, y);
			buffer.addToBuffer(height);
			FixedVector &normal = map.getNormal(x, y);
			buffer.addToBuffer(normal);
		}
	}

	// Send the target data
	std::map<unsigned int, Target *> &possibletargets =
			ScorchedServer::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	buffer.addToBuffer((int) possibletargets.size());
	for (itor = possibletargets.begin();
		itor != possibletargets.end();
		itor++)
	{
		Target *target = (*itor).second;
		buffer.addToBuffer(target->getPlayerId());

		if (target->isTarget())
		{
			if (!target->writeMessage(buffer)) return false;
		}
		else
		{
			if (!((Tank*)target)->writeMessage(buffer, true)) return false;
		}
	}

	return true;
}

#ifndef S3D_SERVER

#include <common/FileLogger.h>
#include <image/ImageFactory.h>
#include <landscape/Landscape.h>

static FileLogger *syncCheckFileLogger = 0;

static int syncCount = 0;

static void syncCheckLog(const std::string &message)
{
	if (!syncCheckFileLogger) 
	{
		char buffer[256];
		snprintf(buffer, 256, "SyncCheckLog-%u-", time(0));
		syncCheckFileLogger = new FileLogger(buffer);
	}	

	LoggerInfo info(message.c_str());
	info.setTime();
	syncCheckFileLogger->logMessage(info);
}

bool ComsSyncCheckMessage::readMessage(NetBufferReader &reader)
{
	syncCount++;

	std::vector<std::string> &clientsyncs = 
		ScorchedClient::instance()->getActionController().getSyncCheck();
	int serverSyncNo = 0, clientSyncNo = (int) clientsyncs.size();
	if (!reader.getFromBuffer(serverSyncNo)) return false;

	bool printOutput = false;
	std::string output;
	for (int s=0; s<MAX(serverSyncNo, clientSyncNo); s++)
	{
		std::string clientsync, serversync;
		if (s < serverSyncNo)
		{
			if (!reader.getFromBuffer(serversync)) return false;
		}
		if (s < clientSyncNo)
		{
			clientsync = clientsyncs[s];
		}

		bool diff = (serversync != clientsync);
		if (diff) printOutput = true;

		output.append(S3D::formatStringBuffer("%i: %s %s ::: %s\n", 
			syncCount,
			(diff?"***":""),
			serversync.c_str(), 
			clientsync.c_str()));
	}
	if (printOutput)
	{
		syncCheckLog(output.c_str());
	}

	// Read the height map data
	HeightMap &map = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap();
	int heightDiffs = 0, normalDiffs = 0;
	bool *heightDiff = new bool[map.getMapHeight() * map.getMapWidth()];
	for (int y=0; y<map.getMapHeight(); y++)
	{
		for (int x=0; x<map.getMapWidth(); x++)
		{
			fixed actualheight = map.getHeight(x, y);
			FixedVector actualnormal = map.getNormal(x, y);
			fixed sentheight;
			FixedVector sentnormal;
			if (!reader.getFromBuffer(sentheight)) return false;
			if (!reader.getFromBuffer(sentnormal)) return false;
			
			if (actualheight != sentheight) 
			{
				syncCheckLog(S3D::formatStringBuffer("%i %i", 
					actualheight.getInternal(), sentheight.getInternal()));
				heightDiffs++;
			}
			if (actualnormal != sentnormal) normalDiffs++;

			heightDiff[x + y * map.getMapWidth()] = (actualheight != sentheight);
		}
	}
	if (heightDiffs > 0 || normalDiffs > 0)
	{
		syncCheckLog(S3D::formatStringBuffer(
			"SyncCheck %i - Height diffs %i, Normal diffs %i",
			syncCount,
			heightDiffs, normalDiffs));
	
		ImageHandle newMap = ImageFactory::createBlank(
			Landscape::instance()->getMainMap().getWidth(),
			Landscape::instance()->getMainMap().getHeight());

		GLubyte *dest = newMap.getBits();
		GLubyte *src = Landscape::instance()->getMainMap().getBits();
		for (int y=0; y<newMap.getHeight(); y++)
		{
			for (int x=0; x<newMap.getWidth(); x++)
			{
				GLubyte r = src[0];
				GLubyte g = src[1];
				GLubyte b = src[2];

				int x2 = (x * map.getMapWidth()) / newMap.getWidth();
				int y2 = (y * map.getMapHeight()) / newMap.getHeight();
				if (heightDiff[x2 + y2 * map.getMapWidth()])
				{
					r = g = b = 255;
				}
		
				dest[0] = r;
				dest[1] = g;
				dest[2] = b;

				dest+=3;
				src+=3;
			}
		}
		Landscape::instance()->getMainTexture().replace(newMap, false);
		Landscape::instance()->setTextureType(Landscape::eOther);
	}

	// Read the target data
	std::set<unsigned int> readTargets;
	int numberTargetsSend = 0;
	NetBuffer tmpBuffer;

	if (!reader.getFromBuffer(numberTargetsSend)) return false;
	for (int i=0; i<numberTargetsSend; i++)
	{
		unsigned int playerId = 0;
		if (!reader.getFromBuffer(playerId)) return false;
		readTargets.insert(playerId);

		Target *target = ScorchedClient::instance()->getTargetContainer().getTargetById(playerId);
		if (!target)
		{
			syncCheckLog(S3D::formatStringBuffer(
				"SyncCheck %i - Failed to find a client target : %u", syncCount, playerId));
			return true;
		}

		tmpBuffer.reset();
		if (target->isTarget())
		{
			target->writeMessage(tmpBuffer);
		}
		else
		{
			((Tank*)target)->writeMessage(tmpBuffer, true);
		}

		bool different = false;
		if (!target->getTargetState().getMovement())
		{
			for (unsigned int i=0; i<tmpBuffer.getBufferUsed(); i++)
			{
				if (reader.getReadSize() + i >= reader.getBufferSize() ||
					tmpBuffer.getBuffer()[i] != reader.getBuffer()[reader.getReadSize() + i])
				{
					syncCheckLog(S3D::formatStringBuffer("SyncCheck %i - Targets values differ : %u:%s, position %i", 
							syncCount, playerId, 
							target->getCStrName().c_str(), i));

					different = true;
					Logger::addLogger(syncCheckFileLogger);

					// Only used for step-through debugging to see where the
					// differences are
					tmpBuffer.setBufferUsed(i);
					NetBufferReader tmpReader(tmpBuffer);
					target->readMessage(tmpReader);

					Logger::remLogger(syncCheckFileLogger);

					break;
				}
			}
		}

		if (target->isTarget())
		{
			static Target *tmpTarget = new Target(0, LangString(),
				ScorchedClient::instance()->getContext());
			if (!tmpTarget->readMessage(reader)) return false;
			tmpTarget->getLife().setLife(0);
		}
		else
		{
			static Tank *tmpTank = new Tank(
				ScorchedClient::instance()->getContext(),
				0,
				0, 
				LangString(),
				Vector::getNullVector(),
				"",
				"");
			if (!tmpTank->readMessage(reader)) return false;
			tmpTank->getState().setState(TankState::sDead);
			tmpTank->getLife().setLife(0);
			tmpTank->getState().setState(TankState::sDead);

			if (different)
			{
				syncCheckLog(S3D::formatStringBuffer("%s %s", 
					tmpTank->getState().getStateString(),
					((Tank*)target)->getState().getStateString()));
			}
		}
	}
	if (reader.getBufferSize() != reader.getReadSize())
	{
		syncCheckLog(S3D::formatStringBuffer("SyncCheck not all bytes read : %i   %i,%i",
			syncCount, reader.getBufferSize(), reader.getReadSize()));
	}

	if (syncCheckFileLogger)
	{
		syncCheckLog(S3D::formatStringBuffer("SyncCheck : %i,%i",
			syncCount,
			ScorchedClient::instance()->getOptionsTransient().getCurrentGameNo()));
	}

	if (syncCheckFileLogger)
	{
		syncCheckLog(S3D::formatStringBuffer("SyncCheck %i checked. (%i syncs)", syncCount, serverSyncNo));
	}
	else
	{
		Logger::log(S3D::formatStringBuffer("SyncCheck %i checked. (%i syncs)", syncCount, serverSyncNo));
	}

	return true;
}

#else // #ifndef S3D_SERVER

bool ComsSyncCheckMessage::readMessage(NetBufferReader &reader)
{
	return true;
}

#endif // #ifndef S3D_SERVER
