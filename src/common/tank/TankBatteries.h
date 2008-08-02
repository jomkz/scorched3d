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

#if !defined(AFX_TANKBATTERIES_H__83501862_9536_4108_A7E6_2377AD98EB72__INCLUDED_)
#define AFX_TANKBATTERIES_H__83501862_9536_4108_A7E6_2377AD98EB72__INCLUDED_

#include <net/NetBuffer.h>

class ScorchedContext;
class Tank;
class TankBatteries  
{
public:
	TankBatteries(ScorchedContext &context);
	virtual ~TankBatteries();

	void setTank(Tank *tank) { tank_ = tank; }

	void newMatch();
	void changed();
	int getNoBatteries();
	bool canUse();

protected:
	ScorchedContext &context_;
	Tank *tank_;

};

#endif // !defined(AFX_TANKBBATTERIES_H__83501862_9536_4108_A7E6_2377AD98EB72__INCLUDED_)
