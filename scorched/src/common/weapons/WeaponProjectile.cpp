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

#include <weapons/AccessoryStore.h>
#include <weapons/WeaponProjectile.h>
#include <actions/ShotProjectile.h>
#include <engine/ActionController.h>
#include <common/Logger.h>

REGISTER_ACCESSORY_SOURCE(WeaponProjectile);

WeaponProjectile::WeaponProjectile() : 
	Weapon("WeaponProjectile", "Makes a projectile that collides with the ground or other targets."),
	apexCollision_("Makes the projectile collide in mid air (at the apex of the shot) instead of with the ground, it will dud if it hits the ground first", 0, false), 
	apexNoDud_("The projectile will not dud if it hits the ground before the apex has been reached", 0, false), 
	timedDud_("The projectile will not dud if it hits the ground before the timeout", 0, false),
	showShotPath_("The projectiles will leave a colored trail visible to the player", 0, false), 
	showEndPoint_("The projectiles will leave a colored mark on the ground visible to the player", 0, false), 
	noCameraTrack_("This projectile will be used for the action camera views", 0, false),
	spinAxis_("The axis the projectile model should spin around", 0, FixedVector(0, 0, 1)), 
	spinSpeed_("WeaponProjectile::spinSpeed", "How fast the projectile rotates", 0, "1"), 
	timedCollision_("WeaponProjectile::timedCollision", "Makes the projectile collide after a number of seconds, it will dud if it hits the ground first", 0, "0"), 
	heightCollision_("WeaponProjectile::heightCollision", "Makes the projectile collide at a certain height", 0, "0"),
	wobbleSpin_("WeaponProjectile::wobbleSpin", "The speed that a projectile will wobble from side to side, good values are < 1.0.  Only used if wobbleamount is given.", 0, "0"), 
	wobbleAmount_("WeaponProjectile::wobbleAmount", "The distance that a projectile will wobble from side to side, good values are < 1.0", 0, "2"),
	shieldHurtFactor_("WeaponProjectile::shieldHurtFactor", "Affects damage the projectile does during shield collision, 1 = 100%", 0, "1"), 
	thrustAmount_("WeaponProjectile::thrustAmount", "Amount of force applied when using thrust", 0, "0"), 
	thrustTime_("WeaponProjectile::thrustTime", "Amount of time to apply thrust", 0, "0"),
	drag_("WeaponProjectile::drag", "Amount of drag placed on the projectile", 0, "0"), 
	stepSize_("This projectile will move every step size seconds", 0, fixed(true, 75))
{
	addChildXMLEntry("wobblespin", &wobbleSpin_);
	addChildXMLEntry("wobbleamount", &wobbleAmount_);
	addChildXMLEntry("spinspeed", &spinSpeed_);
	addChildXMLEntry("spinaxis", &spinAxis_);
	addChildXMLEntry("projectilemodel", &modelId_);
	addChildXMLEntry("drag", &drag_);
	addChildXMLEntry("stepsize", &stepSize_);
	addChildXMLEntry("thrusttime", &thrustTime_);
	addChildXMLEntry("thrustamount", &thrustAmount_);
	addChildXMLEntry("showshotpath", &showShotPath_);
	addChildXMLEntry("showendpoint", &showEndPoint_);
	addChildXMLEntry("apexcollision", &apexCollision_);
	addChildXMLEntry("apexnodud", &apexNoDud_);
	addChildXMLEntry("timedcollision", &timedCollision_);
	addChildXMLEntry("timeddud", &timedDud_);
	addChildXMLEntry("heightcollision", &heightCollision_);
	addChildXMLEntry("nocameratrack", &noCameraTrack_);
	addChildXMLEntry("shieldhurtfactor", &shieldHurtFactor_);
	addChildXMLEntry("localgroupname", &localGroups_);
	addChildXMLEntry("globalgroupname", &globalGroups_);
	addChildXMLEntry("collisionaction", &collisionAction_);
	particleDefinition_.addAllEntries(*this);
}

WeaponProjectile::~WeaponProjectile()
{
}

void WeaponProjectile::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	Action *action = new ShotProjectile(
		position, 
		velocity,
		this, 
		weaponContext,
		spinSpeed_.getValue(context),
		spinAxis_.getValue()); 
	context.getActionController().addAction(action);	
}
