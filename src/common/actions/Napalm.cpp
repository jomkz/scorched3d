////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <target/TargetDamageCalc.h>
#include <target/TargetRenderer.h>
#include <target/TargetState.h>
#include <target/TargetSpace.h>
#include <actions/Napalm.h>
#include <actions/CameraPositionAction.h>
#ifndef S3D_SERVER
	#include <sprites/ExplosionTextures.h>
	#include <sprites/NapalmRenderer.h>
	#include <GLEXT/GLStateExtension.h>
	#include <landscape/Landscape.h>
	#include <landscape/DeformTextures.h>
	#include <landscape/Smoke.h>
	#include <client/ScorchedClient.h>
#endif
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeTex.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>

static const int deformSize = 3;

Napalm::Napalm(int x, int y, Weapon *weapon, 
	NapalmParams *params,
	WeaponFireContext &weaponContext) :
	ActionReferenced("Napalm"),
	x_(x), y_(y), napalmTime_(0), 
	weapon_(weapon), params_(params),
	weaponContext_(weaponContext), hitWater_(false),
	totalTime_(0), hurtTime_(0),
	counter_(0.1f, 0.1f), set_(0)
{
}

Napalm::~Napalm()
{
	delete params_;
}

void Napalm::init()
{
	fixed ShowTime = 5;
	FixedVector position(fixed(x_), fixed(y_), context_->landscapeMaps->
		getGroundMaps().getHeight(x_, y_));
	CameraPositionAction *pos = new CameraPositionAction(
		position, ShowTime, 5);
	context_->actionController->addAction(pos);

#ifndef S3D_SERVER
	if (!context_->serverMode) 
	{
		set_ = ExplosionTextures::instance()->getTextureSetByName(
			params_->getNapalmTexture());
	}
#endif // #ifndef S3D_SERVER
}

std::string Napalm::getActionDetails()
{
	return S3D::formatStringBuffer("%i,%i %s",
		x_, y_, weapon_->getParent()->getName());
}

void Napalm::simulate(fixed frameTime, bool &remove)
{
#ifndef S3D_SERVER
	if (!context_->serverMode)
	{
		if (!params_->getNoSmoke() &&
			counter_.nextDraw(frameTime.asFloat()))
		{
			int count = int(RAND * float(napalmPoints_.size()));

			std::list<Napalm::NapalmEntry *>::iterator itor;
			std::list<Napalm::NapalmEntry *>::iterator endItor = 
				napalmPoints_.end();
			for (itor = napalmPoints_.begin();
					itor != endItor;
					itor++, count--)
			{
				NapalmEntry *entry = (*itor);
				if (count == 0)
				{
					fixed posZ = 
						ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeight(
						entry->posX, entry->posY);
					Landscape::instance()->getSmoke().
						addSmoke(float(entry->posX), float(entry->posY), posZ.asFloat());
					break;
				}
			}
		}
	}
#endif // #ifndef S3D_SERVER

	// Add napalm for the period of the time interval
	// once the time interval has expired then start taking it away
	// Once all napalm has disapeared the simulation is over
	totalTime_ += frameTime;
	while (totalTime_ > params_->getStepTime())
	{
		totalTime_ -= params_->getStepTime();
		napalmTime_ += params_->getStepTime();
		if (napalmTime_ < params_->getNapalmTime())
		{
			// Still within the time period, add more napalm
			simulateAddStep();

			// Check for the case where we land in water
			if (napalmPoints_.empty())
			{
				remove = true;
				break;
			}
		}
		else
		{
			// Not within the time period remove napalm
			if (!napalmPoints_.empty())
			{
				simulateRmStep();
			}
			else
			{
				remove = true;
				break;
			}
		}
	}

	// Calculate how much damage to make to the tanks
	hurtTime_ += frameTime;
	while (hurtTime_ > params_->getHurtStepTime())
	{
		hurtTime_ -= params_->getHurtStepTime();

		simulateDamage();
	}

	Action::simulate(frameTime, remove);
}

fixed Napalm::getHeight(int x, int y)
{
	LandscapeMaps *hmap = context_->landscapeMaps;
	if (x < 0 || y < 0 ||
		x > hmap->getGroundMaps().getLandscapeWidth() ||
		y > hmap->getGroundMaps().getLandscapeHeight())
	{
		// The height at the sides of the landscape is huge
		// so we will never go there with the napalm
		return fixed::MAX_FIXED;
	}

	// Return the correct height the square + the
	// height of all the napalm on this square
	// the napalm builds up and get higher so
	// we can go over small bumps
	return hmap->getGroundMaps().getHeight(x, y) +
		hmap->getGroundMaps().getNapalmHeight(x, y);
}

void Napalm::simulateRmStep()
{
	// Remove the first napalm point from the list
	// and remove the height from the landscape
	NapalmEntry *entry = napalmPoints_.front();
	napalmPoints_.pop_front();
	int x = entry->posX;
	int y = entry->posY;
	delete entry;

	context_->landscapeMaps->getGroundMaps().getNapalmHeight(x, y) -= params_->getNapalmHeight();
}

void Napalm::simulateAddStep()
{
	// Get the height of this point
	fixed height = getHeight(x_, y_);

	if (!params_->getAllowUnderWater())
	{
		// Napalm does not go under water (for now)
		// Perhaps we could add a boiling water sound at some point
		fixed waterHeight = -10;
		LandscapeTex &tex = *context_->landscapeMaps->getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
		{
			LandscapeTexBorderWater *water = 
				(LandscapeTexBorderWater *) tex.border;
       	 	waterHeight = water->height;
		}

		if (height < waterHeight) // Water height
		{
			if (!hitWater_)
			{
				// This is the first time we have hit the water
				hitWater_ = true;
			}
			return;
		}
	} 

	// Add this current point to the napalm map
	RandomGenerator &random = context_->actionController->getRandom();
	int offset = (random.getRandFixed() * 31).asInt();
	NapalmEntry *newEntry = new NapalmEntry(x_, y_, offset);
	napalmPoints_.push_back(newEntry);
#ifndef S3D_SERVER
	if (!context_->serverMode)
	{
		ParticleEmitter emitter;
		emitter.setAttributes(
			params_->getNapalmTime().asFloat(), params_->getNapalmTime().asFloat(),
			0.5f, 1.0f, // Mass
			0.01f, 0.02f, // Friction
			Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
			Vector(1.0f, 1.0f, 1.0f), 0.9f, // StartColor1
			Vector(1.0f, 1.0f, 1.0f), 0.6f, // StartColor2
			Vector(1.0f, 1.0f, 1.0f), 0.0f, // EndColor1
			Vector(1.0f, 1.0f, 1.0f), 0.1f, // EndColor2
			1.5f, 1.5f, 1.5f, 1.5f, // Start Size
			1.5f, 1.5f, 1.5f, 1.5f, // EndSize
			Vector(0.0f, 0.0f, 0.0f), // Gravity
			params_->getLuminance(),
			false);
		Vector position1(float(x_) + 0.5f, float(y_) - 0.2f, 0.0f);
		Vector position2(float(x_) - 0.5f, float(y_) - 0.2f, 0.0f);
		Vector position3(float(x_) + 0.0f, float(y_) + 0.5f, 0.0f);
		emitter.emitNapalm(
			position1, 
			ScorchedClient::instance()->getParticleEngine(),
			set_);
		emitter.emitNapalm(
			position2, 
			ScorchedClient::instance()->getParticleEngine(),
			set_);
		emitter.emitNapalm(
			position3, 
			ScorchedClient::instance()->getParticleEngine(),
			set_);
	}
#endif // #ifndef S3D_SERVER

	/*
	if (!weapon_->getNoObjectDamage())
	{
		context_->landscapeMaps->getGroundMaps().getObjects().burnObjects(
			*context_,
			(unsigned int) x_, (unsigned int) y_,
			playerId_);
		context_->landscapeMaps->getGroundMaps().getObjects().burnObjects(
			*context_,
			(unsigned int) x_ + 1, (unsigned int) y_ + 1,
			playerId_);
		context_->landscapeMaps->getGroundMaps().getObjects().burnObjects(
			*context_,
			(unsigned int) x_ - 1, (unsigned int) y_ - 1,
			playerId_);
	}
	*/

	context_->landscapeMaps->getGroundMaps().getNapalmHeight(x_, y_) += params_->getNapalmHeight();
	height += params_->getNapalmHeight();

#ifndef S3D_SERVER
	if (!context_->serverMode)
	{
		static DeformLandscape::DeformPoints deformMap;
		static bool deformCreated = false;
		if (!deformCreated)
		{
			deformCreated = true;

			Vector center(deformSize + 1, deformSize + 1);
			for (int x=0; x<(deformSize + 1) * 2; x++)
			{
				for (int y=0; y<(deformSize + 1) * 2; y++)
				{
					Vector pos(x, y);
					float dist = (center - pos).Magnitude();
					dist /= deformSize;
					dist = 1.0f - MIN(1.0f, dist);

					DIALOG_ASSERT(x < 100 && y < 100);
					deformMap.map[x][y] = fixed::fromFloat(dist);
				}
			}
		}

		// Add the ground scorch
		if (!GLStateExtension::getNoTexSubImage())
		{
			if (RAND < params_->getGroundScorchPer().asFloat())
			{
				Vector pos(x_, y_);
				DeformTextures::deformLandscape(pos, 
					(int) (deformSize + 1),
					ExplosionTextures::instance()->getScorchBitmap(
						params_->getDeformTexture()),
					deformMap);
			}
		}
	}
#endif // #ifndef S3D_SERVER

	// Calculate every time as the landscape may change
	// due to other actions
	fixed heightL = getHeight(x_-1, y_);
	fixed heightR = getHeight(x_+1, y_);
	fixed heightU = getHeight(x_, y_+1);
	fixed heightD = getHeight(x_, y_-1);

	// Find the new point to move to (if any)
	// This point must be lower than the current point
	if (heightL < height &&
		heightL < heightR &&
		heightL < heightU &&
		heightL < heightD)
	{
		// Move left
		x_ -= 1;
	}
	else if (heightR < height &&
		heightR < heightL &&
		heightR < heightU &&
		heightR < heightD)
	{
		// Move right
		x_ += 1;
	}
	else if (heightU < height &&
		heightU < heightL &&
		heightU < heightR &&
		heightU < heightD)
	{
		// Move up
		y_ += 1;
	}
	else if (heightD < height)
	{
		// Move down
		y_ -= 1;
	}
	else
	{
		// None of the landscape is currently lower than the current point
		// Just wait, as this point will be now be covered in napalm
		// and may get higher and higher until it is
	}
}

void Napalm::simulateDamage()
{
	const int EffectRadius = params_->getEffectRadius();

	// Store how much each tank is damaged
	// Keep in a map so we don't need to create multiple
	// damage actions.  Now we only create one per tank
	static std::map<unsigned int, fixed> TargetDamageCalc;

	// Add damage into the damage map for each napalm point that is near to
	// the tanks
	std::list<NapalmEntry *>::iterator itor;
	std::list<NapalmEntry *>::iterator endItor = 
		napalmPoints_.end();
	for (itor = napalmPoints_.begin();
		itor != endItor;
		itor++)
	{
		NapalmEntry *entry = (*itor);

		fixed height = context_->landscapeMaps->getGroundMaps().
			getHeight(entry->posX, entry->posY);
		FixedVector position(
			fixed(entry->posX), 
			fixed(entry->posY),
			height);

		std::map<unsigned int, Target *> collisionTargets;
		context_->targetSpace->getCollisionSet(position, 
			fixed(EffectRadius), collisionTargets);
		std::map<unsigned int, Target *>::iterator itor;
		for (itor = collisionTargets.begin();
			itor != collisionTargets.end();
			itor++)
		{
			Target *target = (*itor).second;
			if (target->getAlive())
			{
				std::map<unsigned int, fixed>::iterator damageItor = 
					TargetDamageCalc.find(target->getPlayerId());
				if (damageItor == TargetDamageCalc.end())
				{
					TargetDamageCalc[target->getPlayerId()] = params_->getHurtPerSecond();
				}
				else
				{
					TargetDamageCalc[target->getPlayerId()] += params_->getHurtPerSecond();
				}
			}
		}
	}

	// Add all the damage to the tanks (if any)
	if (!TargetDamageCalc.empty())
	{
		std::map<unsigned int, fixed>::iterator damageItor;
		for (damageItor = TargetDamageCalc.begin();
			damageItor != TargetDamageCalc.end();
			damageItor++)
		{
			unsigned int playerId = (*damageItor).first;
			fixed damage = (*damageItor).second;

			Target *target = context_->targetContainer->getTargetById(playerId);
			if (!target) continue;

			// Add damage to the tank
			// If allowed for this target type (mainly for trees)
			if (!target->getTargetState().getNoDamageBurn())
			{
				TargetDamageCalc::damageTarget(*context_, target, weapon_, 
					weaponContext_, damage, true, false, false);
			}

			// Set this target to burnt
			if (target->getRenderer() &&
				!params_->getNoObjectDamage())
			{
				target->getRenderer()->targetBurnt();
			}
		}
		TargetDamageCalc.clear();
	}
}
