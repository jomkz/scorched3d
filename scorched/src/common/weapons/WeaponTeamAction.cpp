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

#include <weapons/WeaponTeamAction.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>

REGISTER_ACCESSORY_SOURCE(WeaponTeamAction);

static XMLEntryEnum::EnumEntry teamEnum[] =
{
	{ "TeamNone", WeaponTeamActionEntry::TeamNone },
	{ "TeamRed", WeaponTeamActionEntry::TeamRed },
	{ "TeamBlue", WeaponTeamActionEntry::TeamBlue },
	{ "TeamGreen", WeaponTeamActionEntry::TeamGreen },
	{ "TeamYellow", WeaponTeamActionEntry::TeamYellow },
	{ "", -1 }
};

WeaponTeamActionEntry::WeaponTeamActionEntry() :
	XMLEntryContainer("WeaponTeamActionEntry", 
		"A weapon that will be activated if the firing tank matches the given team"),
	team_("The team that the firing tank must match to activate the weapon", teamEnum),
	weapon_()
{
	addChildXMLEntry("team", &team_);
	addChildXMLEntry("weapon", &weapon_);
}

WeaponTeamActionEntry::~WeaponTeamActionEntry()
{
}

WeaponTeamActionEntryList::WeaponTeamActionEntryList() :
	XMLEntryList<WeaponTeamActionEntry>(
		"A choice of weapons that will only be activated if the firing tank matches the given team", 1)
{
}

WeaponTeamActionEntryList::~WeaponTeamActionEntryList()
{
}

WeaponTeamActionEntry *WeaponTeamActionEntryList::createXMLEntry()
{
	return new WeaponTeamActionEntry();
}

WeaponTeamAction::WeaponTeamAction() :
	WeaponCallback("WeaponTeamAction", 
		"Used to activate different events based on which team the firing tank is on.  "
		"You can define different events for all 4 teams.")
{
}

WeaponTeamAction::~WeaponTeamAction()
{

}

void WeaponTeamAction::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponTeamAction", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponTeamAction::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTargetContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	std::list<WeaponTeamActionEntry *>::iterator itor = actions_.getChildren().begin(),
		end = actions_.getChildren().end();
	for (;itor!=end;++itor)
	{
		if ((*itor)->team_.getValue() == tank->getTeam())
		{
			Weapon *action = (*itor)->weapon_.getValue();
			action->fire(context, weaponContext, position, velocity);
			break;
		}
	}
}

