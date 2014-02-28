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

#include <weapons/WeaponDelay.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>

REGISTER_ACCESSORY_SOURCE(WeaponDelay);

WeaponDelay::WeaponDelay() :
	WeaponCallback("WeaponDelay", "Delays for a specific amount of time before performing the next action."),
	delay_("WeaponDelay::delay", "Amount of time in seconds to delay before performing the following action"),
	delayedWeapon_()
{
	addChildXMLEntry("delayedweapon", &delayedWeapon_);
	addChildXMLEntry("delay", &delay_);
}

WeaponDelay::~WeaponDelay()
{
}

void WeaponDelay::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	Action *action = new CallbackWeapon(
		"WeaponDelay", 
		this, delay_.getValue(context), 0,
		weaponContext, position, velocity);
	context.getActionController().addAction(action);
}

void WeaponDelay::weaponCallback(
			ScorchedContext &context,
			WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
			unsigned int userData)
{
	delayedWeapon_.getValue()->fire(context, weaponContext, position, velocity);
}
