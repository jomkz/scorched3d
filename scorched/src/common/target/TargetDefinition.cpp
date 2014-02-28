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
#include <common/RandomGenerator.h>
#include <common/DefinesAssert.h>
#ifndef S3D_SERVER
//	#include <tankgraph/TargetRendererImplTarget.h>
#endif
#include <tanket/Tanket.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetDefinition.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetGroup.h>
#include <target/TargetParachute.h>
#include <target/TargetState.h>
#include <target/TargetContainer.h>
#include <tankai/TankAIStore.h>
#include <server/ScorchedServer.h>
#include <common/Defines.h>
#include <XML/XMLNode.h>

TargetDefinition::TargetDefinition() : 
	XMLEntryContainer("TargetDefinition", 
		"Defines all attributes for a new target (a model on the lanscape with physical properties)"),
	name_("The name given to this target, this name may be displayed on the score board or used for debugging", 0, ""),
	ainame_("The name of an AI (bot) to associated with this target, this will turn the target into a computer controlled entity", 0, ""),
	life_("The amount of life that this target will start with", 0, 1), 
	boundingsphere_("If a bounding sphere should be used for collision detection rather than a box", 0, true),
	driveovertodestroy_("Can this target be destroyed by a player tank driving over it", 0, false), 
	flattendestroy_("Is this target destroyed when the surrounding area is flattened, e.g. by a tank placement or a falling tank", 0, false), 
	border_("The minimum distance between targets when they are automaticaly placed", 0, 0), 
	displaydamage_("Does this target have damage numbers displayed over it when it is damaged", 0, true), 
	displayshadow_("Does this target cast shadows", 0, true), 
	nodamageburn_("Does this target burn when affected by weapons such as napalm", 0, false), 
	nocollision_("Can weapons hit this target", 0, false), 
	nofalling_("Does this target fall when the earth underneath is removed", 0, false),
	nofallingdamage_("Does this target take damage from falling (if it can fall)", 0, false), 
	billboard_("Does this target always get rotated to the viewing camera", 0, false), 
	team_("The team to add this target to, 0 is no team", 0, 0), 
	parachute_("The name of a parachute accessory that this target will start with activated", 0, ""),
	shield_("The name of a shield accessory that this target with start with activated", 0, ""), 
	useNormalMoves_("If this target has an associated AI does the target take a turn at making a move, or can it make moves anytime", 0, true),
	removeaction_("The accessory name to fire when this target is removed (destroyed)", 0, ""),
	burnaction_("The accessory name to fire when this target is burned", 0, ""),
	collisionaction_("The accessory name to fire when a projectile collides with this target", 0, ""),
	groups_(),
	modelId_("The model of the target")
{
	addChildXMLEntry("model", &modelId_);
	addChildXMLEntry("name", &name_);
	addChildXMLEntry("life", &life_);
	addChildXMLEntry("boundingsphere", &boundingsphere_);
	addChildXMLEntry("driveovertodestroy", &driveovertodestroy_, "flattendestroy", &flattendestroy_);
	addChildXMLEntry("nocollision", &nocollision_, "nodamageburn", &nodamageburn_, "nofalling", &nofalling_);
	addChildXMLEntry("displaydamage", &displaydamage_, "displayshadow", &displayshadow_);
	addChildXMLEntry("nofallingdamage", &nofallingdamage_);
	addChildXMLEntry("usenormalmoves", &useNormalMoves_);
	addChildXMLEntry("billboard", &billboard_);
	addChildXMLEntry("parachute", &parachute_, "shield", &shield_);
	addChildXMLEntry("ainame", &ainame_);
	addChildXMLEntry("team", &team_);
	addChildXMLEntry("border", &border_);
	addChildXMLEntry("removeaction", &removeaction_, "burnaction", &burnaction_, "collisionaction", &collisionaction_);
	addChildXMLEntry("groups", &groups_);
}

TargetDefinition::~TargetDefinition()
{
}

Target *TargetDefinition::createTarget(unsigned int playerId,
	FixedVector &position,
	FixedVector &velocity,
	ScorchedContext &context,
	RandomGenerator &generator)
{
	Target *target = 0;
	Tanket *tanket = 0;
	if (ainame_.getValue().empty())
	{
		target = new Target(playerId, LANG_STRING(name_.getValue().c_str()), context);
	} 
	else
	{
		tanket = new Tanket(context, playerId, 0, LANG_STRING(name_.getValue()));
		TankAI *ai = 0;
		if (context.getServerMode() &&
			ainame_.getValue() != "Human")
		{
			ai = ((ScorchedServer &)context).getTankAIs().getAIByName(ainame_.getValue().c_str());
			if (!ai) 
			{
				S3D::dialogExit("TargetDefinition",
					S3D::formatStringBuffer("Cannot find AI name %s", ainame_.getValue().c_str()));
			}
			ai = ai->createCopy(tanket);
		}
		tanket->setTankAI(ai);
		tanket->getShotInfo().setUseNormalMoves(useNormalMoves_.getValue());
		target = tanket;
	}
	target->getLife().setBoundingSphere(boundingsphere_.getValue());


	target->getTargetState().setNoCollision(nocollision_.getValue());
	target->getTargetState().setDisplayDamage(displaydamage_.getValue());
	target->getTargetState().setDisplayShadow(displayshadow_.getValue());
	target->getTargetState().setNoDamageBurn(nodamageburn_.getValue());
	target->getTargetState().setNoFalling(nofalling_.getValue());
	target->getTargetState().setNoFallingDamage(nofallingdamage_.getValue());
	target->getTargetState().setDriveOverToDestroy(driveovertodestroy_.getValue());
	target->getTargetState().setFlattenDestroy(flattendestroy_.getValue());
	target->getLife().setMaxLife(life_.getValue());
	// target->getLife().setSize(finalSize.getValue()); // TODO
	target->getLife().setVelocity(velocity);
	//target->getLife().setRotation(rotation.getValue()); // TODO
	target->setBorder(border_.getValue());
	target->loaded();
	if (tanket) tanket->newMatch();
	target->newGame();

	if (shield_.getValue().c_str()[0] && 0 != strcmp(shield_.getValue().c_str(), "none"))
	{
		Accessory *shield = context.getAccessoryStore().
			findByPrimaryAccessoryName(shield_.getValue().c_str());
		if (!shield)
		{
			S3D::dialogExit("Scorched3D",
				S3D::formatStringBuffer("Failed to find shield named \"%s\"",
				shield_.getValue().c_str()));
		}

		target->getShield().setCurrentShield(shield);
	}

	if (parachute_.getValue().c_str()[0] && 0 != strcmp(parachute_.getValue().c_str(), "none"))
	{
		Accessory *parachute = context.getAccessoryStore().
			findByPrimaryAccessoryName(parachute_.getValue().c_str());
		if (!parachute)
		{
			S3D::dialogExit("Scorched3D",
				S3D::formatStringBuffer("Failed to find parachute named \"%s\"",
				parachute_.getValue().c_str()));
		}

		target->getParachute().setCurrentParachute(parachute);
	}
 
	if (removeaction_.getValue().c_str()[0] && 0 != strcmp(removeaction_.getValue().c_str(), "none"))
	{
		Accessory *action = context.getAccessoryStore().
			findByPrimaryAccessoryName(removeaction_.getValue().c_str());		
		if (!action || action->getType() != AccessoryPart::AccessoryWeapon)
		{
			S3D::dialogExit("Scorched3D",
				S3D::formatStringBuffer("Failed to find death action \"%s\"",
				removeaction_.getValue().c_str()));
		}

		target->setDeathAction(action);
	}
	if (burnaction_.getValue().c_str()[0] && 0 != strcmp(burnaction_.getValue().c_str(), "none"))
	{
		Accessory *action = context.getAccessoryStore().
			findByPrimaryAccessoryName(burnaction_.getValue().c_str());		
		if (!action || action->getType() != AccessoryPart::AccessoryWeapon)
		{
			S3D::dialogExit("Scorched3D",
				S3D::formatStringBuffer("Failed to find burn action \"%s\"",
				burnaction_.getValue().c_str()));
		}

		target->setBurnAction(action);
	}
	if (collisionaction_.getValue().c_str()[0] && 0 != strcmp(collisionaction_.getValue().c_str(), "none"))
	{
		Accessory *action = context.getAccessoryStore().
			findByPrimaryAccessoryName(collisionaction_.getValue().c_str());		
		if (!action || action->getType() != AccessoryPart::AccessoryWeapon)
		{
			S3D::dialogExit("Scorched3D",
				S3D::formatStringBuffer("Failed to find collision action \"%s\"",
				collisionaction_.getValue().c_str()));
		}

		target->setCollisionAction(action);
	}

#ifndef S3D_SERVER
/*	if (!context.getServerMode())
	{
		target->setRenderer(
			new TargetRendererImplTarget(
				target, modelId_, modelburntId_,
				finalModelScale.asFloat(), finalBrightness.asFloat(), 
				billboard_));
	}
*/
#endif // #ifndef S3D_SERVER

	target->getLife().setTargetPosition(position);
	groups_.addToGroups(context.getObjectGroups(), &target->getGroup());

	context.getTargetContainer().addTarget(target);

	return target;
}
