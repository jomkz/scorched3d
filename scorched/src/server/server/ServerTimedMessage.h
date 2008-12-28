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

#if !defined(__INCLUDE_ServerTimedMessageh_INCLUDE__)
#define __INCLUDE_ServerTimedMessageh_INCLUDE__

#include <lang/LangString.h>
#include <list>
#include <time.h>

class ServerTimedMessage
{
public:
	ServerTimedMessage();
	virtual ~ServerTimedMessage();

	void simulate();

protected:
	struct TimedMessageEntry
	{
		TimedMessageEntry() : lastTime(0) {}
		unsigned int timeInterval;
		time_t lastTime;
		std::list<LangString> messages;
	};

	time_t lastCheckTime_, lastReadTime_;
	std::list<TimedMessageEntry> entries_;

	bool load();
	void checkEntries(time_t currentTime);

};

#endif

