////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
	createSmoke_(true),	createFlame_(true), 
	spinSpeed_(1), apexNoDud_(false), timedDud_(false),
	timedCollision_(0), shieldHurtFactor_(1), windFactor_(1),
	flameLife_(1.0f), smokeLife_(4.0f),
	flameStartColor1_(0.9f, 0.0f, 0.0f), flameStartColor2_(1.0f, 0.2f, 0.2f),
	flameEndColor1_(0.95f, 0.9f, 0.2f), flameEndColor2_(1.0f, 1.0f, 0.3f),
	flameStartSize_(0.5f), flameEndSize_(3.0f),
	smokeStartSize_(0.5f), smokeEndSize_(4.0f),
	thrustAmount_(0), thrustTime_(0),
	drag_(0),
	engineSound_("data/wav/misc/rocket.wav"),
	scale_(1), flareType_(0)
{

}

WeaponProjectile::~WeaponProjectile()
{

}

bool WeaponProjectile::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the accessory under
	XMLNode *underNode = 0;
	accessoryNode->getNamedChild("under", underNode, false);
	if (underNode) under_ = true;

	// Get the spin
	accessoryNode->getNamedChild("spinspeed", spinSpeed_, false);

	// Get the optional weapon model scale
	accessoryNode->getNamedChild("projectilescale", scale_, false);

	// Get the optional weapon model
	XMLNode *modelNode = 0;
	if (accessoryNode->getNamedChild("projectilemodel", modelNode, false))
	{
		if (!modelId_.initFromNode("data/accessories", modelNode)) return false;
	}

	// Get smoke life
	accessoryNode->getNamedChild("smokelife", smokeLife_, false);
	accessoryNode->getNamedChild("flamelife", flameLife_, false);
	accessoryNode->getNamedChild("flamestartsize", flameStartSize_, false);
	accessoryNode->getNamedChild("flameendsize", flameEndSize_, false);
	accessoryNode->getNamedChild("smokestartsize", smokeStartSize_, false);
	accessoryNode->getNamedChild("smokeendsize", smokeEndSize_, false);

	// flame color
	accessoryNode->getNamedChild("flamestartcolor1", flameStartColor1_, false);
	accessoryNode->getNamedChild("flamestartcolor2", flameStartColor2_, false);
	accessoryNode->getNamedChild("flameendcolor1", flameEndColor1_, false);
	accessoryNode->getNamedChild("flameendcolor2", flameEndColor2_, false);

	// Drag
	accessoryNode->getNamedChild("drag", drag_, false);

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

	// Get the no smoke node
	XMLNode *noCreateSmokeNode = 0;
	accessoryNode->getNamedChild("nocreatesmoke", noCreateSmokeNode, false);
	if (noCreateSmokeNode) createSmoke_ = false;

	// Get the no smoke node
	XMLNode *noCreateFlameNode = 0;
	accessoryNode->getNamedChild("nocreateflame", noCreateFlameNode, false);
	if (noCreateFlameNode) createFlame_ = false;

	// Get the engine sound (if any)
	accessoryNode->getNamedChild("enginesound", engineSound_, false);

	// Get the flare type (if any)
	accessoryNode->getNamedChild("flaretype", flareType_, false);

	// Get the hurt factor (if any)
	accessoryNode->getNamedChild("shieldhurtfactor", shieldHurtFactor_, false);

	// Get the wind factor (if any)
	accessoryNode->getNamedChild("windfactor", windFactor_, false);

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
		flareType_, // FlareType
		spinSpeed_.getValue(context)); 
	context.getActionController().addAction(action);	
}
