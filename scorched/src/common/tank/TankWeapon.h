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

#if !defined(AFX_TANKWEAPON_H__CC20069B_6E50_49E0_B735_D739BCBF58A9__INCLUDED_)
#define AFX_TANKWEAPON_H__CC20069B_6E50_49E0_B735_D739BCBF58A9__INCLUDED_

#include <map>
#include <list>
#include <string>
#include <net/NetBuffer.h>

class ScorchedContext;
class Tank;
class Accessory;
class TankWeapon  
{
public:
	TankWeapon(ScorchedContext &context);
	virtual ~TankWeapon();

	void setTank(Tank *tank) { tank_ = tank; }

	void newMatch();
	void changed();

	bool setWeapon(Accessory *wp);

	// Change the currently selected weapon
	Accessory *getCurrent();
	const char *getWeaponString();

protected:
	Accessory *currentWeapon_;
	ScorchedContext &context_;
	Tank *tank_;

	void setCurrentWeapon(Accessory *wp);

};

#endif // !defined(AFX_TANKWEAPON_H__CC20069B_6E50_49E0_B735_D739BCBF58A9__INCLUDED_)
