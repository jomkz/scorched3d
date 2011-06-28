////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <actions/TankDamage.h>
#include <actions/TankFalling.h>
#include <actions/TankSay.h>
#include <actions/CameraPositionAction.h>
#include <actions/Resurrection.h>
#ifndef S3D_SERVER
	#include <sprites/TextActionRenderer.h>
#endif
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <common/StatsLogger.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Shield.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <tank/TankContainer.h>
#include <tank/TankTeamScore.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <tankai/TankAI.h>
#include <target/TargetShield.h>
#include <target/TargetLife.h>
#include <target/TargetParachute.h>
#include <target/TargetState.h>
#include <tankai/TankAIStrings.h>
#include <lang/LangResource.h>

TankDamage::TankDamage(Weapon *weapon, 
		unsigned int damagedPlayerId, WeaponFireContext &weaponContext,
		fixed damage, bool useShieldDamage, bool checkFall,
		bool shieldOnlyDamage) :
	Action(weaponContext.getPlayerId()),
	weapon_(weapon), firstTime_(true),
	damagedPlayerId_(damagedPlayerId), weaponContext_(weaponContext),
	damage_(damage), useShieldDamage_(useShieldDamage), checkFall_(checkFall),
	shieldOnlyDamage_(shieldOnlyDamage)
{
}

TankDamage::~TankDamage()
{
}

void TankDamage::init()
{
	if (!context_->getServerMode()) 
	{
		Target *damagedTarget = 
			context_->getTargetContainer().getTargetById(damagedPlayerId_);
		if (damagedTarget && !damagedTarget->isTarget())
		{
			TankViewPointProvider *vPoint = new TankViewPointProvider();
			vPoint->setValues(damagedTarget->getLife().getTargetPosition());
			CameraPositionAction *pos = new CameraPositionAction(
				weaponContext_.getPlayerId(),
				vPoint,
				4,
				15,
				false);
			context_->getActionController().addAction(pos);
		}
	}
}

std::string TankDamage::getActionDetails()
{
	return S3D::formatStringBuffer("%u %s %s",
		damagedPlayerId_, damage_.asQuickString(), weapon_->getParent()->getName());
}

void TankDamage::simulate(fixed frameTime, bool &remove)
{
	if (firstTime_)
	{
		firstTime_ = false;
		calculateDamage();
	}

	remove = true;
	Action::simulate(frameTime, remove);
}

void TankDamage::calculateDamage()
{
	unsigned int firedPlayerId = weaponContext_.getPlayerId();

	// Find the tank that has been damaged
	Target *damagedTarget = 
		context_->getTargetContainer().getTargetById(damagedPlayerId_);
	if (!damagedTarget || !damagedTarget->getAlive()) return;

	// Tell this tanks ai that is has been hurt by another tank
	if (!damagedTarget->isTarget())
	{
		// Tell all AIs about this collision
		std::map<unsigned int, Tank *> tanks = 
			context_->getTankContainer().getAllTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			TankAI *ai = tank->getTankAI();
			if (ai)
			{		
				if (tank->getState().getState() == TankState::sNormal)
				{
					ai->tankHurt(weapon_, damage_.asFloat(),
						damagedTarget->getPlayerId(), 
						firedPlayerId);
				}
			}
		}
	}

	// Add the collision action
	addDamageAction(damagedTarget, damagedTarget->getCollisionAction());

	// Remove any damage from shield first
	if (damage_ > 0)
	{
		fixed shieldDamage = 0;
		Accessory *sh = damagedTarget->getShield().getCurrentShield();
		if (sh && useShieldDamage_)
		{
			Shield *shield = (Shield *) sh->getAction();
			fixed shieldPowerRequired = 
				damage_ * shield->getHitPenetration();
			fixed shieldPower = 
				damagedTarget->getShield().getShieldPower();
			if (shieldPower > shieldPowerRequired)
			{
				shieldPower -= shieldPowerRequired;
				damage_ = 0;
			}
			else
			{
				fixed p = shieldPower / shield->getHitPenetration();
				shieldPower = 0;
				damage_ -= p;
			}

			damagedTarget->getShield().setShieldPower(shieldPower);
		}
	}

	// Remove the remaining damage from the tank
	if (damage_ > 0 && !shieldOnlyDamage_)
	{
#ifndef S3D_SERVER
		if (!context_->getServerMode() &&
			damagedTarget->getTargetState().getDisplayDamage())
		{
			Vector position = damagedTarget->getLife().getFloatPosition();
			position[0] += RAND * 5.0f - 2.5f;
			position[1] += RAND * 5.0f - 2.5f;
			position[2] += RAND * 5.0f - 2.5f;

			Vector redColor(0.75f, 0.0f, 0.0f);
			context_->getActionController().addAction(
				new SpriteAction(
					new TextActionRenderer(
						position,
						redColor,
						S3D::formatStringBuffer("%.0f", damage_.asFloat()))));
		}
#endif // #ifndef S3D_SERVER

		// Remove the life
		if (damage_ > damagedTarget->getLife().getLife()) damage_ = 
			damagedTarget->getLife().getLife();
		damagedTarget->getLife().setLife(damagedTarget->getLife().getLife() - damage_);
		if (context_->getOptionsGame().getLimitPowerByHealth() &&
			!damagedTarget->isTarget())
		{
			Tank *damagedTank = (Tank *) damagedTarget;
			damagedTank->getPosition().changePower(0, true);
		}

		if (context_->getOptionsGame().getActionSyncCheck())
		{
			context_->getSimulator().addSyncCheck(
				S3D::formatStringBuffer("TankDamage: %u %s", 
					damagedTarget->getPlayerId(),
					damagedTarget->getLife().getLife().asQuickString()));
		}

		// Check if the tank is dead
		bool killedTank = (damagedTarget->getLife().getLife() == 0);

		// Add any score got from this endevour
		// Should always be a tank that has fired
		Tank *firedTank = 
			context_->getTankContainer().getTankById(firedPlayerId);
		if (firedTank && !damagedTarget->isTarget())
		{	
			Tank *damagedTank = (Tank *) damagedTarget;

			// Add this tank as a tank that assisted in the kill
			damagedTank->getScore().getHurtBy().insert(firedTank->getPlayerId());

			// Calculate team kills
			bool selfKill = (damagedPlayerId_ ==  firedPlayerId);
			bool teamKill = ((context_->getOptionsGame().getTeams() > 1) &&
				(firedTank->getTeam() == damagedTank->getTeam()));

			if (!killedTank)
			{
				// Calculate money won for not killing this tank
				int moneyPerHit = 
					context_->getOptionsGame().getMoneyWonPerHitPoint() *
						weapon_->getArmsLevel();
				if (context_->getOptionsGame().getMoneyPerHealthPoint()) 
					moneyPerHit = (moneyPerHit * damage_.asInt()) / 100;
				if (selfKill || teamKill) moneyPerHit *= -1;

				firedTank->getScore().setMoney(
					firedTank->getScore().getMoney() + moneyPerHit);
			}
			else 
			{
				int moneyPerKill = 
					context_->getOptionsGame().getMoneyWonPerKillPoint() *
						weapon_->getArmsLevel();
				if (!selfKill && !teamKill)
				{
					// Note this is done before turn kills is updated
					// so for the first kill turn kills will be 0
					// i.e. no multikill bonus for 1st kill
					moneyPerKill +=
						context_->getOptionsGame().getMoneyWonPerMultiKillPoint() *
						weapon_->getArmsLevel() *
						weaponContext_.getInternalContext().getKillCount();
				}
				if (context_->getOptionsGame().getMoneyPerHealthPoint()) 
					moneyPerKill = (moneyPerKill * damage_.asInt()) / 100;
				int scorePerKill = context_->getOptionsGame().getScorePerKill();

				int moneyPerAssist = 
					context_->getOptionsGame().getMoneyWonPerAssistPoint() *
						weapon_->getArmsLevel();
				int scorePerAssist = context_->getOptionsGame().getScorePerAssist();

				// Update kills and score
				if (selfKill || teamKill)
				{
					firedTank->getScore().setKills(
						firedTank->getScore().getKills() - 1);
					firedTank->getScore().setMoney(
						firedTank->getScore().getMoney() - moneyPerKill);
					firedTank->getScore().setScore(
						firedTank->getScore().getScore() - scorePerKill);

					if (firedTank->getTeam() > 0)
					{
						context_->getTankTeamScore().addScore(
							-scorePerKill, firedTank->getTeam());
					}
				}
				else
				{
					firedTank->getScore().setKills(
						firedTank->getScore().getKills() + 1);
					firedTank->getScore().setMoney(
						firedTank->getScore().getMoney() + moneyPerKill);
					firedTank->getScore().setScore(
						firedTank->getScore().getScore() + scorePerKill);

					weaponContext_.getInternalContext().setKillCount(
						weaponContext_.getInternalContext().getKillCount() + 1);

					if (firedTank->getTeam() > 0)
					{
						context_->getTankTeamScore().addScore(
							scorePerKill, firedTank->getTeam());
					}
				}

				// Update assists
				std::set<unsigned int> &hurtBy = 
					damagedTank->getScore().getHurtBy();
				std::set<unsigned int>::iterator itor;
				for (itor = hurtBy.begin();
					itor != hurtBy.end();
					itor++)
				{
					unsigned int hurtByPlayer = (*itor);
					Tank *hurtByTank = 
						context_->getTankContainer().getTankById(hurtByPlayer);
					if (!hurtByTank) continue;

					// Only score when the tank does not hurt itself
					if (hurtByTank == damagedTank) continue;

					// You don't get an assist for your kill
					if (hurtByTank == firedTank) continue;

					// or a team member
					if ((context_->getOptionsGame().getTeams() > 1) &&
						(hurtByTank->getTeam() == damagedTank->getTeam())) continue;

					// Update assist score
					hurtByTank->getScore().setAssists(
						hurtByTank->getScore().getAssists() + 1);
					hurtByTank->getScore().setMoney(
						hurtByTank->getScore().getMoney() + moneyPerAssist);
					hurtByTank->getScore().setScore(
						hurtByTank->getScore().getScore() + scorePerAssist);

					if (hurtByTank->getTeam() > 0)
					{
						context_->getTankTeamScore().addScore(
							scorePerAssist, hurtByTank->getTeam());
					}
				}
			}
		}

		if (killedTank)
		{
			// The tank has died, make it blow up etc.
			calculateDeath();

			if (!damagedTarget->isTarget())
			{
				// The tank is now dead
				Tank *damagedTank = (Tank *) damagedTarget;
				damagedTank->getState().setState(TankState::sDead);

				// This tank has lost a life
				if (damagedTank->getState().getMaxLives() > 0)
				{
					damagedTank->getState().setLives(
						damagedTank->getState().getLives() - 1);
				}
				Resurrection::checkResurection(context_, damagedTank);
			}
		}
	}

	// Check if the tank needs to fall
	if (checkFall_ && damagedTarget->getAlive())
	{
		// The tank is not dead check if it needs to fall
		FixedVector &position = damagedTarget->getLife().getTargetPosition();
		if (context_->getLandscapeMaps().getGroundMaps().
			getInterpHeight(position[0], position[1]) < position[2])
		{
			// Check this tank is not already falling
			if (!damagedTarget->getTargetState().getFalling())
			{
				Parachute *parachute = 0;
				Accessory *paraAccessory = 
					damagedTarget->getParachute().getCurrentParachute();
				if (paraAccessory)
				{
					parachute = (Parachute *) paraAccessory->getAction();
				}

				// Tank falling
				context_->getActionController().addAction(
					new TankFalling(weapon_, damagedPlayerId_, weaponContext_,
						parachute));
			}
		}
	}

	// DO LAST
	// If the tank is a target, remove the target
	if (!damagedTarget->getAlive() &&
		damagedTarget->isTarget())
	{
		Target *removedTarget = 
			context_->getTargetContainer().
				removeTarget(damagedTarget->getPlayerId());
		if (context_->getOptionsGame().getActionSyncCheck())
		{
			context_->getSimulator().addSyncCheck(
				S3D::formatStringBuffer("RemoveTarget : %u %s", 
					removedTarget->getPlayerId(),
					removedTarget->getCStrName().c_str()));
		}

		delete removedTarget;
	}
}

void TankDamage::calculateDeath()
{
	Target *killedTarget = 
		context_->getTargetContainer().getTargetById(damagedPlayerId_);
	if (!killedTarget) return;

	// Log the death
	logDeath();

	// Add the tank death explosion
	// Make the tank explode in one of many ways
	addDamageAction(killedTarget, killedTarget->getDeathAction());
}

void TankDamage::addDamageAction(Target *target, Weapon *weapon) 
{
	if (weapon)
	{
		if (context_->getOptionsGame().getActionSyncCheck())
		{
			context_->getSimulator().addSyncCheck(
				S3D::formatStringBuffer("DeathAction: %s", 
					weapon->getParent()->getName()));
		}

		FixedVector position = target->getLife().getTargetPosition();
		FixedVector velocity;
		WeaponFireContext weaponContext(weaponContext_.getPlayerId(), 
			Weapon::eDataDeathAnimation);
		weapon->fireWeapon(*context_, weaponContext, 
			position, velocity);
		StatsLogger::instance()->weaponFired(weapon, true);
	}
}

void TankDamage::logDeath()
{
	unsigned int firedPlayerId = weaponContext_.getPlayerId();

	Target *killedTarget = 
		context_->getTargetContainer().getTargetById(damagedPlayerId_);
	if (killedTarget->isTarget()) return;

	Tank *killedTank = (Tank *) killedTarget;

	if (killedTank->getDestinationId() == 0)
	{
		const char *line = context_->getTankAIStrings().getDeathLine(*context_);
		if (line)
		{
			context_->getActionController().addAction(
				new TankSay(killedTank->getPlayerId(), 
				LANG_STRING(line)));
		}
	}

	Tank *firedTank = 0;
	if (firedPlayerId != 0) firedTank = context_->getTankContainer().getTankById(firedPlayerId);
	else
	{
		Vector white(1.0f, 1.0f, 1.0f);
		static Tank envTank(*context_, 0, 0, 
			LANG_STRING("Environment"), 
			white);
		envTank.setUniqueId("Environment");
		firedTank = &envTank;
	}

	if (firedTank)
	{
		if (damagedPlayerId_ == firedPlayerId)
		{
			int skillChange = context_->getOptionsGame().getSkillForSelfKill();
			fixed weight = (fixed(weapon_->getArmsLevel()) / 10) + 1;
			skillChange = (fixed(skillChange) * weight).asInt();

			firedTank->getScore().setSkill(firedTank->getScore().getSkill() + skillChange);

			StatsLogger::instance()->
				tankSelfKilled(firedTank, weapon_);
			StatsLogger::instance()->
				weaponKilled(weapon_, (weaponContext_.getData() & Weapon::eDataDeathAnimation));
			{
				ChannelText text("combat",
					LANG_RESOURCE_3(
						"TANK_KILLED_SELF", 
						"[p:{0}] killed self with a [w:{1}] ({2} skill)",
						firedTank->getTargetName(),
						weapon_->getParent()->getName(),
						S3D::formatStringBuffer("%i", skillChange)));
				ChannelManager::showText(*context_, text);
			}
		}
		else if ((context_->getOptionsGame().getTeams() > 1) &&
				(firedTank->getTeam() == killedTank->getTeam())) 
		{
			int skillChange = context_->getOptionsGame().getSkillForTeamKill();
			fixed weight = (fixed(weapon_->getArmsLevel()) / 10) + 1;
			skillChange = (fixed(skillChange) * weight).asInt();

			firedTank->getScore().setSkill(firedTank->getScore().getSkill() + skillChange);

			StatsLogger::instance()->
				tankTeamKilled(firedTank, killedTank, weapon_);
			StatsLogger::instance()->
				weaponKilled(weapon_, (weaponContext_.getData() & Weapon::eDataDeathAnimation));
			{
				ChannelText text("combat", 
					LANG_RESOURCE_4(
						"TANK_KILLED_TEAM",
						"[p:{0}] team killed [p:{1}] with a [w:{2}] ({3} skill)",
						firedTank->getTargetName(),
						killedTank->getTargetName(),
						weapon_->getParent()->getName(),
						S3D::formatStringBuffer("%i", skillChange)));
				ChannelManager::showText(*context_, text);
			}
		}
		else
		{
			int kbonus = 0; // Killer
			int vbonus = 0; // Victim
			if (firedTank->getPlayerId() != 0 && killedTank->getPlayerId() != 0) 
			{
				int kskill = firedTank->getScore().getSkill();
				int vskill = killedTank->getScore().getSkill();

				if (kskill > vskill) 
				{
					// killer is better than the victim
					kbonus = ((kskill + vskill)*(kskill + vskill)) / (kskill*kskill);
					vbonus = kbonus * vskill / (vskill + kskill);
				} 
				else 
				{
					// the victim is better than the killer
					kbonus = ((vskill + kskill)*(vskill + kskill)) / (vskill*vskill) * vskill / kskill;
					vbonus = kbonus * (vskill + 1000) / (vskill + kskill);
				}

				if (vbonus > context_->getOptionsGame().getMaxSkillLost())
				{
					vbonus = context_->getOptionsGame().getMaxSkillLost();
				}
				if (kbonus > context_->getOptionsGame().getMaxSkillGained())
				{
					kbonus = context_->getOptionsGame().getMaxSkillGained();
				}

				//$vbonus = $vskill if $vbonus > $vskill;
				//$kbonus = $kskill if $kbonus > $kskill;
				fixed weight = (fixed(weapon_->getArmsLevel()) / 10) + 5;
				kbonus = (fixed(kbonus) * weight).asInt();
				vbonus = (fixed(vbonus) * weight).asInt();
			}
			firedTank->getScore().setSkill(firedTank->getScore().getSkill() + kbonus);
			killedTank->getScore().setSkill(killedTank->getScore().getSkill() - vbonus);

			StatsLogger::instance()->
				tankKilled(firedTank, killedTank, weapon_);
			StatsLogger::instance()->
				weaponKilled(weapon_, (weaponContext_.getData() & Weapon::eDataDeathAnimation));
			{
				if (weaponContext_.getInternalContext().getKillCount() > 1)
				{
					ChannelText text("combat", 
						LANG_RESOURCE_5(
						"TANK_KILLED_MULTIOTHER",
						"[p:{0}] multi-killed [p:{1}] with a [w:{2}] ({3}, {4} skill)",
						firedTank->getTargetName(),
						killedTank->getTargetName(),
						weapon_->getParent()->getName(),
						S3D::formatStringBuffer("%i", kbonus),
						S3D::formatStringBuffer("%i", -vbonus)));
					ChannelManager::showText(*context_, text);
				}
				else
				{
					ChannelText text("combat", 
						LANG_RESOURCE_5(
						"TANK_KILLED_OTHER",
						"[p:{0}] killed [p:{1}] with a [w:{2}] ({3}, {4} skill)",
						firedTank->getTargetName(),
						killedTank->getTargetName(),
						weapon_->getParent()->getName(),
						S3D::formatStringBuffer("%i", kbonus),
						S3D::formatStringBuffer("%i", -vbonus)));
					ChannelManager::showText(*context_, text);
				}
			}
		}
	}
}
