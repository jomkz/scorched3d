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

#include <weapons/WeaponMoveTank.h>
#include <weapons/AccessoryStore.h>
#include <actions/TanketMovement.h>
#include <engine/ActionController.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefn.h>

REGISTER_ACCESSORY_SOURCE(WeaponMoveTank);

WeaponMoveTank::WeaponMoveTank() :
	Weapon("WeaponMoveTank", "Moves the players tank to the current position. "
		"The player will gradually move across the landscape at a configurable speed. "
		"This primitive can configured to use the accessory as 'fuel' so that the range is limited by the amount of that item the player has. "
		"If the item is not used as fuel, the maximumrange is always whatever the current max range is defined as."),
	stepTime_("Used to determine rate at which the player moves. Lower = faster movement", 0, fixed(true, 500)), 
	useFuel_("-1 = \"true\"= Use fuel from inventory."
           "0 = \"false\"= Don't use fuel from inventory."
           "1+ = Use this much fuel from inventory.", 0, -1), 
	maximumRange_("Maximum distance the player can move per turn", 0, 100)
{
	addChildXMLEntry("steptime", &stepTime_);
	addChildXMLEntry("maximumrange", &maximumRange_);
	addChildXMLEntry("usefuel", &useFuel_);
}

WeaponMoveTank::~WeaponMoveTank()
{

}

void WeaponMoveTank::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	int arenaX = context.getLandscapeMaps().getDescriptions().getDefn()->getArenaX();
	int arenaY = context.getLandscapeMaps().getDescriptions().getDefn()->getArenaY();
	int arenaWidth = context.getLandscapeMaps().getDescriptions().getDefn()->getArenaWidth();
	int arenaHeight = context.getLandscapeMaps().getDescriptions().getDefn()->getArenaHeight();

	int posX = position[0].asInt();
	int posY = position[1].asInt();
	if (posX > arenaX + 5 && posX < arenaX + arenaWidth - 5 &&
		posY > arenaY + 5 && posY < arenaY + arenaHeight - 5)
	{
		context.getActionController().addAction(
			new TanketMovement(weaponContext, this, posX, posY));		
	}
}
