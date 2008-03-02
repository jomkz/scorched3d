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

#ifndef _TankModelContainer_h
#define _TankModelContainer_h

#include <net/NetBuffer.h>

// The model used for this tank
class ScorchedContext;
class TankType;
class Tank;
class TankModelContainer
{
public:
	TankModelContainer(const char *modelName, 
		const char *typeName);
	virtual ~TankModelContainer();

	void setTank(Tank *tank) { tank_ = tank; }

	// The name of the model that should be used for this tank
	const char *getTankModelName() { return tankModelName_.c_str(); }
	const char *getTankOriginalModelName() { return tankOriginalModelName_.c_str(); }
	const char *getTankTypeName() { return tankTypeName_.c_str(); }
	void setTankModelName(
		const char *modelName, 
		const char *originalModelName,
		const char *typeName);

	// Serialize the modelid
    bool writeMessage(NetBuffer &buffer);
    bool readMessage(NetBufferReader &reader);

protected:
	Tank *tank_;
	std::string tankModelName_; // Model for tank (this is the one the server also has)
	std::string tankOriginalModelName_; // The original tank model sent from the client
	std::string tankTypeName_;

};

#endif // _TankModelContainer_h
