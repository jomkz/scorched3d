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

#include <weapons/WeaponLaser.h>
#include <weapons/AccessoryStore.h>
#include <actions/Laser.h>
#include <engine/ActionController.h>

REGISTER_ACCESSORY_SOURCE(WeaponLaser);

WeaponLaser::WeaponLaser() :
	Weapon("WeaponLaser", "Shoots a laser beam in the direction currently being aimed at.  The longer the laser beam (the power) the less damage it will do"),
	totalTime_("WeaponLaser::totalTime", "The total time to show the laser for", 0, "1"),
	minimumHurt_("WeaponLaser::minimumHurt", "The amount of damage that will be taken from the target if the laser is at its minimum strength (maximum length)"), 
	maximumHurt_("WeaponLaser::maximumHurt", "The amount of damage that will be taken from the target if the laser is at its maximum strength (minimum length)"),
	minimumDistance_("WeaponLaser::minimumDistance", "Minimum distance the laser can travel (minimum length)"), 
	maximumDistance_("WeaponLaser::maximumDistance", "Maximum distance the laser can travel (maximum length)"),
	hurtRadius_("WeaponLaser::hurtRadius", "Radius from the center of the laser beam within which tanks will take damage from the laser"),
	hurtFirer_("Whether or not the laser can hurt the firer of the weapon", 0, false)
{
	addChildXMLEntry("totaltime", &totalTime_);
	addChildXMLEntry("minimumhurt", &minimumHurt_);
	addChildXMLEntry("maximumhurt", &maximumHurt_);
	addChildXMLEntry("hurtradius", &hurtRadius_);
	addChildXMLEntry("minimumdistance", &minimumDistance_);
	addChildXMLEntry("maximumdistance", &maximumDistance_);
	addChildXMLEntry("hurtfirer", &hurtFirer_);
}

WeaponLaser::~WeaponLaser()
{
}

void WeaponLaser::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new Laser(this, position, velocity, weaponContext));
}
