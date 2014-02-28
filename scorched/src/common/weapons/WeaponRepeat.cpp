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

#include <weapons/WeaponRepeat.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>

REGISTER_ACCESSORY_SOURCE(WeaponRepeat);

WeaponRepeat::WeaponRepeat() : 
	WeaponCallback("WeaponRepeat", 
		"Performs the given xml a specified number of times with a specified delay in between. "),
	delay_("WeaponRepeat::delay", "Number of seconds to delay between repeats", 0, "0"), 
	repeat_("The number of times to repeat the action")
{
	addChildXMLEntry("repeat", &repeat_);
	addChildXMLEntry("delay", &delay_);
	addChildXMLEntry("repeatweapon", &repeatWeapon_);
}

WeaponRepeat::~WeaponRepeat()
{
}

void WeaponRepeat::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	if (delay_.getValue(context) == 0)
	{
		for (int i=0; i<repeat_.getValue(); i++)
		{
			repeatWeapon_.getValue()->fire(context, weaponContext, position, velocity);
		}
	}
	else
	{
		weaponCallback(context, weaponContext, position, velocity, repeat_.getValue());
	}
}

void WeaponRepeat::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	repeatWeapon_.getValue()->fire(context, weaponContext, position, velocity);

	if (userData > 1)
	{
		context.getActionController().addAction(
			new CallbackWeapon("WeaponRepeat", this, delay_.getValue(context), userData - 1, 
				weaponContext, position, velocity));
	}
}
