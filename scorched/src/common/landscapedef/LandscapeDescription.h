////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#if !defined(__INCLUDE_LandscapeDescriptionh_INCLUDE__)
#define __INCLUDE_LandscapeDescriptionh_INCLUDE__

#include <coms/ComsMessage.h>
#include <string>

class LandscapeDescription
{
public:
	LandscapeDescription();
	LandscapeDescription(
		const std::string &tex, 
		const std::string &defn,
		unsigned int seed,
		const std::string &name,
		unsigned int descriptionNumber);

	const char *getTex() { return tex_.c_str(); }
	const char *getDefn() { return defn_.c_str(); }
	const char *getName() { return name_.c_str(); }
	unsigned int getSeed() { return seed_; }
	unsigned int getDescriptionNumber() { return descriptionNumber_; }

	bool writeMessage(NetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	unsigned int descriptionNumber_;
	std::string tex_;
	std::string defn_;
	std::string name_;
	unsigned int seed_;
};

#endif

