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

#include <weapons/WeaponTypeAction.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <tanket/Tanket.h>
#include <tanket/TanketType.h>

REGISTER_ACCESSORY_SOURCE(WeaponTypeAction);

WeaponTypeActionEntry::WeaponTypeActionEntry() :
	XMLEntryContainer("WeaponTypeActionEntry", 
		"A weapon that will only be activated if fired by the matching tank type", 
		false),
	type_("The tank type that must match for the weapon to activate"),
	weapon_()
{
	addChildXMLEntry("type", &type_);
	addChildXMLEntry("weapon", &weapon_);
}

WeaponTypeActionEntry::~WeaponTypeActionEntry()
{
}

WeaponTypeActionEntryList::WeaponTypeActionEntryList() :
	XMLEntryList<WeaponTypeActionEntry>("A set of weapons that will only be activated when fired by a tank of the matching type", 1)
{
}

WeaponTypeActionEntryList::~WeaponTypeActionEntryList()
{
}

WeaponTypeActionEntry *WeaponTypeActionEntryList::createXMLEntry()
{
	return new WeaponTypeActionEntry();
}

WeaponTypeAction::WeaponTypeAction() :
	WeaponCallback("WeaponTypeAction", 
		"Used to trigger different events based on which tank type the player is using.")
{
}

WeaponTypeAction::~WeaponTypeAction()
{

}

void WeaponTypeAction::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponTypeAction", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponTypeAction::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tanket *tanket = context.getTargetContainer().getTanketById(weaponContext.getPlayerId());
	if (!tanket) return;

	std::list<WeaponTypeActionEntry *>::iterator itor = actions_.getChildren().begin(),
		end = actions_.getChildren().end();
	for (;itor!=end;++itor)
	{
		if ((*itor)->type_.getValue() == tanket->getTanketType()->getName())
		{
			(*itor)->weapon_.getValue()->fire(context, weaponContext, position, velocity);
			break;
		}
	}
}

