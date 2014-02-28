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

#include <tanket/Tanket.h>
#include <tank/TankLib.h>
#include <tanket/TanketShotInfo.h>
#include <tanket/TanketType.h>
#include <target/TargetLife.h>
#include <target/TargetRenderer.h>
#include <engine/ScorchedContext.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>

TanketShotInfo::TanketShotInfo(ScorchedContext &context) :
	maxPower_(1000),
	tanket_(0), context_(context),
	moveId_(0), missedMoves_(0), skippedShots_(0),
	useNormalMoves_(true)
{
}

TanketShotInfo::~TanketShotInfo()
{
}

void TanketShotInfo::newGame()
{
	TanketType *type = tanket_->getTanketType();
	maxPower_ = type->getPower();
	missedMoves_ = 0;
	skippedShots_ = 0;
	if (!context_.getServerMode()) moveId_ = 0;
}

void TanketShotInfo::setMaxPower(fixed power)
{ 
	maxPower_ = power; 
	if (tanket_->getRenderer())
	{
		tanket_->getRenderer()->changed();
	}
}

fixed TanketShotInfo::getMaxOverallPower()
{
	fixed result = maxPower_;
	if (context_.getOptionsGame().getLimitPowerByHealth())
	{
		fixed maxPosPower = 
			tanket_->getLife().getLife() / tanket_->getLife().getMaxLife() * maxPower_;
		if (maxPosPower < result) result = maxPosPower;
	}
	return result;
}

bool TanketShotInfo::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TanketShotInfo");

	buffer.addToBufferNamed("maxPower", maxPower_);
	return true;
}

bool TanketShotInfo::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(maxPower_)) return false;
	return true;
}
