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

#include <actions/AddTarget.h>
#include <engine/ActionController.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <target/TargetContainer.h>
#include <target/TargetDamageCalc.h>
#include <tankai/TankAIAdder.h>
#include <weapons/AccessoryStore.h>
#include <weapons/WeaponAddTarget.h>
#include <weapons/Shield.h>
#include <common/RandomGenerator.h>
#include <common/OptionsScorched.h>

AddTarget::AddTarget(FixedVector &position,
	WeaponAddTarget *addTarget) :
	ActionReferenced("AddTarget"),
	position_(position),
	addTarget_(addTarget)
{

}

AddTarget::~AddTarget()
{
}

void AddTarget::init()
{
}

void AddTarget::simulate(fixed frameTime, bool &remove)
{
	unsigned int playerId = TankAIAdder::getNextTargetId(*context_);

	Target *target = addTarget_->getTargetDefinition().createTarget(
		playerId, position_, FixedVector::getNullVector(), *context_, 
		context_->getSimulator().getRandomGenerator());
	context_->getTargetContainer().addTarget(target);

	if (context_->getOptionsGame().getActionSyncCheck())
	{
		context_->getActionController().addSyncCheck(
			S3D::formatStringBuffer("AddTarget : %u %s %i,%i,%i", 
				playerId,
				target->getCStrName().c_str(),
				position_[0].getInternal(),
				position_[1].getInternal(),
				position_[2].getInternal()));
	}

	// Check if this new target can fall
	WeaponFireContext weaponContext(0, 0);
	TargetDamageCalc::damageTarget(*context_, target, addTarget_, 
		weaponContext, 0, false, true, false);

	remove = true;
	Action::simulate(frameTime, remove);
}

std::string AddTarget::getActionDetails()
{
	return addTarget_->getParent()->getName();
}
