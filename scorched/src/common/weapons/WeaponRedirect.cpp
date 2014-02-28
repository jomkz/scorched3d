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

#include <weapons/WeaponRedirect.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <common/Defines.h>
#include <tank/TankLib.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponRedirect);

WeaponRedirect::WeaponRedirect() :
	Weapon("WeaponRedirect", "Used to re-aim projectiles and other weapons either relative to their current direction or to a specific angle."),
	hredirect_("WeaponRedirect::hredirect", "If habs is true:weapon is re-aimed to the angle specified in hredirect", 0, "0"), 
	vredirect_("WeaponRedirect::vredirect", "If vabs is true:weapon is re-aimed to the angle specified in vredirect", 0, "0"), 
	habs_("Whether or not the horizontal angle adjustment will be to a pre-set angle or a change to current angle (absolute or relative)", 0, false), 
	vabs_("Whether or not the vertical angle adjustment will be to a pre-set angle or a change to current angle (absolute or relative)", 0, false)
{
	addChildXMLEntry("hredirect", &hredirect_);
	addChildXMLEntry("habs", &habs_);
	addChildXMLEntry("vredirect", &vredirect_);
	addChildXMLEntry("vabs", &vabs_);
	addChildXMLEntry("nextaction", &nextAction_);
}

WeaponRedirect::~WeaponRedirect()
{

}

void WeaponRedirect::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	fixed currentMag = velocity.Magnitude();
	fixed currenth = (atan2x(velocity[1], velocity[0]) / fixed::XPI * 180) - 90;
	fixed dist = (velocity[0] * velocity[0] + velocity[1] * velocity[1]).sqrt();
	fixed currentv = atan2x(dist, velocity[2]) / fixed::XPI * 180;

	if (habs_.getValue()) currenth = hredirect_.getValue(context);	// call NumberParser::getValue
	else currenth += hredirect_.getValue(context);		// to evaluate at runtime
	if (vabs_.getValue()) currentv = vredirect_.getValue(context);
	else currentv += vredirect_.getValue(context);
	
	FixedVector newVelocity;
	TankLib::getVelocityVector(newVelocity, currenth, fixed(90) - currentv);
	newVelocity.StoreNormalize();
	newVelocity *= currentMag;
	
	nextAction_.getValue()->fire(context, weaponContext, position, newVelocity);
}

