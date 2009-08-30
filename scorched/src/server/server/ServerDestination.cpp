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

#include <server/ServerDestination.h>
#include <engine/ModFiles.h>

ServerDestinationMod::ServerDestinationMod() : 
	readyToReceive_(true), sent_(false), init_(false), totalLeft_(0)
{
	files_ = new std::list<ModIdentifierEntry>();
}

ServerDestinationMod::~ServerDestinationMod()
{
	delete files_;
	files_ = 0;
}

void ServerDestinationMod::addFile(ModIdentifierEntry &entry)
{
	files_->push_back(entry);
}

void ServerDestinationMod::rmFile(const char *file)
{
	std::list<ModIdentifierEntry>::iterator itor;
	for (itor = files_->begin();
		itor != files_->end();
		itor++)
	{
		ModIdentifierEntry &entry = (*itor);
		if (0 == strcmp(entry.fileName.c_str(), file))
		{
			files_->erase(itor);
			return;
		}
	}
}

ModIdentifierEntry *ServerDestinationMod::getFile(const char *file)
{
	std::list<ModIdentifierEntry>::iterator itor;
	for (itor = files_->begin();
		itor != files_->end();
		itor++)
	{
		ModIdentifierEntry &entry = (*itor);
		if (0 == strcmp(entry.fileName.c_str(), file))
		{
			return &entry;
		}
	}
	return 0;
}

static struct AllowedStateTransitions
{
	ServerDestination::State from, to;
}
allowedStateTransitions[] =
{
	ServerDestination::sDownloadingMod, ServerDestination::sInitializingMod,
	ServerDestination::sInitializingMod, ServerDestination::sLoadingLevel,
	ServerDestination::sLoadingLevel, ServerDestination::sFinished,
	ServerDestination::sFinished, ServerDestination::sLoadingLevel
};

ServerDestination::ServerDestination(unsigned int destinationId, 
									 unsigned int ipAddress) :
	state_(sDownloadingMod),
	adminTries_(0), 
	admin_(false), levelNumber_(0),
	destinationId_(destinationId),
	ipAddress_(ipAddress),
	ping_(25, fixed(true, FIXED_RESOLUTION / 25))
{
}

ServerDestination::~ServerDestination()
{
}

void ServerDestination::setState(State s)
{
	for (int i=0; i<sizeof(allowedStateTransitions) / 
			sizeof(AllowedStateTransitions); i++)
	{
		if (state_ == allowedStateTransitions[i].from &&
			s == allowedStateTransitions[i].to)
		{
			state_ = s;
			break;
		}
	}
	if (state_ == sLoadingLevel) lastSentPingTime_ = 0;
}
