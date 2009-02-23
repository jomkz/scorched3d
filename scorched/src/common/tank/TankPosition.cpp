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

#include <tank/Tank.h>
#include <tank/TankLib.h>
#include <tank/TankType.h>
#include <tank/TankModelStore.h>
#include <tank/TankPosition.h>
#include <tank/TankModelContainer.h>
#include <target/TargetLife.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>

TankPosition::TankPosition(ScorchedContext &context) :
	turretRotXY_(0), turretRotYZ_(0),
	oldTurretRotXY_(0), oldTurretRotYZ_(0),
	power_(1000), oldPower_(1000),
	maxPower_(1000),
	tank_(0), context_(context),
	selectPositionX_(0), selectPositionY_(0)
{
	// Only make the very first shot random angle
	oldTurretRotXY_ = turretRotXY_ = 0;
	oldTurretRotYZ_ = turretRotYZ_ = 45;
}

TankPosition::~TankPosition()
{
}

std::vector<TankPosition::ShotEntry> &TankPosition::getOldShots()
{
	std::vector<ShotEntry>::iterator itor;
	for (itor = oldShots_.begin();
		itor != oldShots_.end();
		itor++)
	{
		ShotEntry &entry = *itor;
		entry.current = 
			(entry.ele == oldTurretRotYZ_ &&
			entry.rot == oldTurretRotXY_ &&
			entry.power == oldPower_);
	}

	return oldShots_; 
}

void TankPosition::newGame()
{
	TankType *type = context_.getTankModels().getTypeByName(
		tank_->getModelContainer().getTankTypeName());

	maxPower_ = type->getPower();
}

void TankPosition::clientNewGame()
{
	TankType *type = context_.getTankModels().getTypeByName(
		tank_->getModelContainer().getTankTypeName());

	maxPower_ = type->getPower();
	oldPower_ = power_ = maxPower_;
	oldShots_.clear();
	madeShot();
}

void TankPosition::madeShot()
{
	oldPower_ = power_;
	oldTurretRotXY_ = turretRotXY_;
	oldTurretRotYZ_ = turretRotYZ_;

	if (oldShots_.empty() ||
		oldShots_.back().power != power_ ||
		oldShots_.back().rot != turretRotXY_ ||
		oldShots_.back().ele != turretRotYZ_)
	{
		oldShots_.push_back(ShotEntry(power_, turretRotXY_, turretRotYZ_));
	}

	if (oldShots_.size() > 15) 
	{
		oldShots_.erase(oldShots_.begin());
	}
}

void TankPosition::revertSettings(unsigned int index)
{
	if (index < oldShots_.size())
	{
		int newIndex = (int) (oldShots_.size() - 1) - index;
		rotateGunXY(oldShots_[newIndex].rot, false);
		rotateGunYZ(oldShots_[newIndex].ele, false);
		changePower(oldShots_[newIndex].power, false);

		oldPower_ = power_;
		oldTurretRotXY_ = turretRotXY_;
		oldTurretRotYZ_ = turretRotYZ_;
	}
}

void TankPosition::undo()
{
	rotateGunXY(oldTurretRotXY_, false);
	rotateGunYZ(oldTurretRotYZ_, false);
	changePower(oldPower_, false);
}

FixedVector &TankPosition::getTankGunPosition()
{
	static FixedVector tankGunPosition;
	tankGunPosition = TankLib::getGunPosition(
			getRotationGunXY(), getRotationGunYZ());
	tankGunPosition += getTankTurretPosition();

	return tankGunPosition;
}

FixedVector &TankPosition::getTankTurretPosition()
{
	static FixedVector tankTurretPosition;
	tankTurretPosition = getTankPosition();
	tankTurretPosition[2] += 1;//model_->getTurretHeight();

	return tankTurretPosition;
}

FixedVector &TankPosition::getTankPosition()
{ 
	return tank_->getLife().getTargetPosition();
}

FixedVector &TankPosition::getVelocityVector()
{
	return TankLib::getVelocityVector(
		getRotationGunXY(), getRotationGunYZ());
}

fixed TankPosition::rotateGunXY(fixed angle, bool diff)
{
	if (diff) turretRotXY_ += angle;
	else turretRotXY_ = angle;

	if (turretRotXY_ <= 0) turretRotXY_ = turretRotXY_ + 360;
	else if (turretRotXY_ > 360) turretRotXY_ = turretRotXY_ - 360;

	return turretRotXY_;
}

fixed TankPosition::rotateGunYZ(fixed angle, bool diff)
{
	if (diff) turretRotYZ_ += angle;
	else turretRotYZ_ = angle;

	if (turretRotYZ_ < 0) turretRotYZ_ = 0;
	else if (turretRotYZ_ > 90) turretRotYZ_ = 90;

	return turretRotYZ_;
}

fixed TankPosition::changePower(fixed power, bool diff)
{
	if (diff) power_ += power;
	else power_ = power;

	if (power_ < 0) power_ = 0;
	if (context_.getOptionsGame().getLimitPowerByHealth())
	{
		fixed maxPosPower = 
			tank_->getLife().getLife() / tank_->getLife().getMaxLife() * maxPower_;
		if (power_ > maxPosPower) power_ = maxPosPower;
	}
	if (power_ > maxPower_) power_ = maxPower_;

	return power_;
}

fixed TankPosition::getRotationXYDiff()
{
	fixed rotDiff = (fixed(360) - turretRotXY_) - (fixed(360) - oldTurretRotXY_);
	if (rotDiff > 180) rotDiff -= 360;
	else if (rotDiff < -180) rotDiff += 360;
	return rotDiff;
}

fixed TankPosition::getRotationYZDiff()
{
	return turretRotYZ_ - oldTurretRotYZ_;
}

fixed TankPosition::getPowerDiff()
{
	return power_ - oldPower_;
}

const char *TankPosition::getRotationString()
{
	static char messageBuffer[255];
	fixed rotDiff = getRotationXYDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 
		(fixed(360) - getRotationGunXY()).asFloat(),
		rotDiff.asFloat());
	return messageBuffer;
}

const char *TankPosition::getElevationString()
{
	static char messageBuffer[255];
	fixed rotDiff = getRotationYZDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 
		getRotationGunYZ().asFloat(),
		rotDiff.asFloat());
	return messageBuffer;
}

const char *TankPosition::getPowerString()
{
	static char messageBuffer[255];
	fixed powDiff = getPowerDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 		
		getPower().asFloat(),
		powDiff.asFloat());
	return messageBuffer;
}

bool TankPosition::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(maxPower_);
	return true;
}

bool TankPosition::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(maxPower_)) return false;
	return true;
}
