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

#ifndef _TankModelContainer_h
#define _TankModelContainer_h

#include <net/NetBuffer.h>

// The model used for this tank
class ScorchedContext;
class TankType;
class TankModel;
class Tank;
class TankModelContainer
{
public:
	TankModelContainer(ScorchedContext &context);
	virtual ~TankModelContainer();

	void setTank(Tank *tank) { tank_ = tank; }

	TankModel *getTankModel();

	void setTankModelName(const char *modelName);

	// Serialize the modelid
    bool writeMessage(NetBuffer &buffer);
    bool readMessage(NetBufferReader &reader);

	void toString(std::string &str);

protected:
	ScorchedContext &context_;
	Tank *tank_;
	std::string modelName_;
	TankModel *tankModel_;
};

#endif // _TankModelContainer_h
