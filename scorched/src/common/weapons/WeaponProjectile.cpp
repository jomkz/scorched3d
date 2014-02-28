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
	under_(false), collisionAction_(0), 
	apexCollision_(false), waterCollision_(false), wallCollision_(true),
	showShotPath_(false), showEndPoint_(false), 
	landscapeCollision_(true), shieldCollision_(true),
	noCameraTrack_(false),
	spinSpeed_("WeaponProjectile::spinSpeed", 1), spinAxis_(0, 0, 1), apexNoDud_(false), timedDud_(false),
	timedCollision_("WeaponProjectile::timedCollision", 0), heightCollision_("WeaponProjectile::heightCollision", 0),
	wobbleSpin_("WeaponProjectile::wobbleSpin", 0), wobbleAmount_("WeaponProjectile::wobbleAmount", 2),
	shieldHurtFactor_("WeaponProjectile::shieldHurtFactor", 1), windFactor_("WeaponProjectile::windFactor", 1), 
	gravityFactor_("WeaponProjectile::gravityFactor", 1),
	thrustAmount_("WeaponProjectile::thrustAmount", 0), thrustTime_("WeaponProjectile::thrustTime", 0),
	drag_("WeaponProjectile::drag", 0), stepSize_(true, 75),
	engineSound_("rocket.wav"),
	scale_("WeaponProjectile::scale", 1)
{

}

WeaponProjectile::~WeaponProjectile()
{
	delete collisionAction_;
	collisionAction_ = 0;
}

bool WeaponProjectile::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the accessory under
	XMLNode *underNode = 0;
	accessoryNode->getNamedChild("under", underNode, false);
	if (underNode) under_ = true;

	// Get the wobble
	accessoryNode->getNamedChild("wobblespin", wobbleSpin_, false);
	accessoryNode->getNamedChild("wobbleamount", wobbleAmount_, false);
	
	// Get the spin
	accessoryNode->getNamedChild("spinspeed", spinSpeed_, false);
	accessoryNode->getNamedChild("spinaxis", spinAxis_, false);

	// Get the optional weapon model scale
	accessoryNode->getNamedChild("projectilescale", scale_, false);

	// Get the optional weapon model
	XMLNode *modelNode = 0;
	if (accessoryNode->getNamedChild("projectilemodel", modelNode, false))
	{
		if (!modelId_.initFromNode(modelNode)) return false;
	}

	// Drag
	accessoryNode->getNamedChild("drag", drag_, false);
	accessoryNode->getNamedChild("stepsize", stepSize_, false);

	// Thrust
	accessoryNode->getNamedChild("thrusttime", thrustTime_, false);
	accessoryNode->getNamedChild("thrustamount", thrustAmount_, false);

	// Get the smoke trails
	XMLNode *smokeNode = 0;
	accessoryNode->getNamedChild("showshotpath", smokeNode, false);
	if (smokeNode) showShotPath_ = true;

	// Get the end point
	XMLNode *endPointNode = 0;
	accessoryNode->getNamedChild("showendpoint", endPointNode, false);
	if (endPointNode) showEndPoint_ = true;
	
	// Get the apex point
	XMLNode *apexNode = 0, *apexNoDudNode = 0;
	accessoryNode->getNamedChild("apexcollision", apexNode, false);
	accessoryNode->getNamedChild("apexnodud", apexNoDudNode, false);
	if (apexNode) apexCollision_ = true;
	if (apexNoDudNode) apexNoDud_ = true;

	// Water collision
	XMLNode *waterNode = 0;
	accessoryNode->getNamedChild("watercollision", waterNode, false);
	if (waterNode) waterCollision_ = true;	

	// Wall collision
	XMLNode *wallCollNode = 0;
	accessoryNode->getNamedChild("nowallcollision", wallCollNode, false);
	if (wallCollNode) wallCollision_ = false;	

	// Get the timed collision point
	XMLNode *timedDudNode = 0;
	accessoryNode->getNamedChild("timedcollision", timedCollision_, false);
	accessoryNode->getNamedChild("timeddud", timedDudNode, false);
	if (timedDudNode) timedDud_ = true;

	// Get the height collision point
	accessoryNode->getNamedChild("heightcollision", heightCollision_, false);

	accessoryNode->getNamedChild("landscapecollision", landscapeCollision_, false);
	accessoryNode->getNamedChild("shieldcollision", shieldCollision_, false);

	accessoryNode->getNamedChild("nocameratrack", noCameraTrack_, false);

	// Get the engine sound (if any)
	accessoryNode->getNamedChild("enginesound", engineSound_, false);

	// Get the hurt factor (if any)
	accessoryNode->getNamedChild("shieldhurtfactor", shieldHurtFactor_, false);

	// Get the wind factor (if any)
	accessoryNode->getNamedChild("windfactor", windFactor_, false);
	accessoryNode->getNamedChild("gravityfactor", gravityFactor_, false);

	// Groups
	if (!localGroups_.readXML(accessoryNode, "localgroupname")) return false;
	if (!globalGroups_.readXML(accessoryNode, "globalgroupname")) return false;

	// Get the next weapon
	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("collisionaction", subNode)) return false;

	// Check next weapon is correct type
	AccessoryPart *accessory = context.getAccessoryStore().
		createAccessoryPart(context, parent_, subNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return false;
	}
	collisionAction_ = (Weapon*) accessory;

	return true;
}

fixed WeaponProjectile::getWindFactor(ScorchedContext &context)
{
	return windFactor_.getValue(context);
}

fixed WeaponProjectile::getGravityFactor(ScorchedContext &context)
{
	return gravityFactor_.getValue(context);
}

fixed WeaponProjectile::getShieldHurtFactor(ScorchedContext &context)
{
	return shieldHurtFactor_.getValue(context);
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
		spinAxis_); 
	context.getActionController().addAction(action);	
}
