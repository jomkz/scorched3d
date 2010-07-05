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

#if !defined(__INCLUDE_LandscapeDefinitionh_INCLUDE__)
#define __INCLUDE_LandscapeDefinitionh_INCLUDE__

#include <coms/ComsMessage.h>
#include <string>

class LandscapeDefinition
{
public:
	LandscapeDefinition();
	LandscapeDefinition(
		const std::string &tex, 
		const std::string &defn,
		unsigned int seed,
		const std::string &name,
		unsigned int definitionNumber);

	const char *getTex() { return tex_.c_str(); }
	const char *getDefn() { return defn_.c_str(); }
	const char *getName() { return name_.c_str(); }
	unsigned int getSeed() { return seed_; }
	unsigned int getDefinitionNumber() { return definitionNumber_; }

	bool writeMessage(NetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	unsigned int definitionNumber_;
	std::string tex_;
	std::string defn_;
	std::string name_;
	unsigned int seed_;
};

#endif

