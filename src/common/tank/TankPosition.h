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

#if !defined(__INCLUDE_TankPositionh_INCLUDE__)
#define __INCLUDE_TankPositionh_INCLUDE__

#include <net/NetBuffer.h>
#include <vector>

class Tank;
class ScorchedContext;
class TankPosition
{
public:

	struct ShotEntry
	{
		ShotEntry(fixed p = 0, fixed r = 0, fixed e = 0) : 
			power(p), rot(r), ele(e) { }
	
		fixed power;
		fixed rot;
		fixed ele;
		bool current;
	};

	TankPosition(ScorchedContext &context);
	virtual ~TankPosition();

	void setTank(Tank *tank) { tank_ = tank; }

	// State change
	void clientNewGame();
	void madeShot();
	void newGame();
	
	// Saved settings
	fixed getRotationXYDiff();
	fixed getRotationYZDiff();
	fixed getPowerDiff();
	void revertSettings(unsigned int index = 0);
	void undo();
	std::vector<ShotEntry> &getOldShots();

	// Gun/Turret Rotation
	fixed rotateGunXY(fixed angle, bool diff=true);
	fixed rotateGunYZ(fixed angle, bool diff=true);
	fixed getRotationGunXY() { return turretRotXY_; }
	fixed getRotationGunYZ() { return turretRotYZ_; }

	// Select position
	int getSelectPositionX() { return selectPositionX_; }
	int getSelectPositionY() { return selectPositionY_; }
	void setSelectPosition(int x, int y) { 
		selectPositionX_ = x; selectPositionY_ = y; }

	// Power of gun
	fixed getPower() { return power_; }
	fixed changePower(fixed power, bool diff=true);

	fixed getMaxPower() { return maxPower_; }
	void setMaxPower(fixed power) { maxPower_ = power; }

	// Position
	FixedVector &getVelocityVector();
	FixedVector &getTankPosition(); // Position of center bottom of tank
	FixedVector &getTankTurretPosition(); // Position of center of turret
	FixedVector &getTankGunPosition(); // Position of end of gun

	const char *getRotationString();
	const char *getElevationString();
	const char *getPowerString();

	// Serialize the tank
	bool writeMessage(NetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	ScorchedContext &context_;
	Tank *tank_;

	// Turret angles
	std::vector<ShotEntry> oldShots_;
	fixed turretRotXY_, turretRotYZ_, power_;
	fixed oldTurretRotXY_, oldTurretRotYZ_, oldPower_;
	fixed maxPower_;
	int selectPositionX_, selectPositionY_;
};

#endif

