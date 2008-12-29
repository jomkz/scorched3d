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

#if !defined(AFX_PARACHUTE_H__F934B35F_3195_45F7_A6B4_D10CCDB296BD__INCLUDED_)
#define AFX_PARACHUTE_H__F934B35F_3195_45F7_A6B4_D10CCDB296BD__INCLUDED_

#include <weapons/AccessoryPart.h>

class Parachute : public AccessoryPart
{
public:
	Parachute();
	virtual ~Parachute();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	FixedVector &getSlowForce() { return slowForce_; }

	REGISTER_ACCESSORY_HEADER(Parachute, AccessoryPart::AccessoryParachute);

protected:
	FixedVector slowForce_;
};

#endif // !defined(AFX_PARACHUTE_H__F934B35F_3195_45F7_A6B4_D10CCDB296BD__INCLUDED_)
