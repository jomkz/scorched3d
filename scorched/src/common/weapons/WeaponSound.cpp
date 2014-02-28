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

#include <weapons/WeaponSound.h>
#include <engine/ActionController.h>
#include <actions/SoundAction.h>
#include <common/Defines.h>
#include <stdlib.h>

REGISTER_ACCESSORY_SOURCE(WeaponSound);

WeaponSoundList::WeaponSoundList() :
	XMLEntryList<XMLEntrySoundID>("A list of sound definitions", 1)
{
}

WeaponSoundList::~WeaponSoundList()
{
}

XMLEntrySoundID *WeaponSoundList::createXMLEntry()
{
	return new XMLEntrySoundID(false);
}

WeaponSound::WeaponSound() :
	Weapon("WeaponSound", "Plays a random sound from the list of given sound definitions")
{
	addChildXMLEntry("SoundID", &soundList_);
}

WeaponSound::~WeaponSound()
{

}

void WeaponSound::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new SoundAction(position, this));
}
