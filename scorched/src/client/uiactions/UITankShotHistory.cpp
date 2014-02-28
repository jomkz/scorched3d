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

#include <uiactions/UITankShotHistory.h>
#include <common/Defines.h>

UITankShotHistory::UITankShotHistory() :
	oldValues_(1000, 0, 45), currentValues_(1000, 0, 45)
{
}

UITankShotHistory::~UITankShotHistory()
{
}

std::vector<UITankShotHistory::ShotEntry> &UITankShotHistory::getOldShots()
{
	std::vector<ShotEntry>::iterator itor;
	for (itor = oldShots_.begin();
		itor != oldShots_.end();
		++itor)
	{
		ShotEntry &entry = *itor;
		entry.current = 
			(entry.ele == oldValues_.ele &&
			entry.rot == oldValues_.rot &&
			entry.power == oldValues_.power);
	}

	return oldShots_; 
}

void UITankShotHistory::clientNewGame()
{
	oldShots_.clear();
	madeShot();
}

void UITankShotHistory::madeShot()
{
	oldValues_ = currentValues_;
	if (oldShots_.empty() ||
		oldShots_.back().power != currentValues_.power ||
		oldShots_.back().rot != currentValues_.rot ||
		oldShots_.back().ele != currentValues_.ele)
	{
		oldShots_.push_back(currentValues_);
	}

	if (oldShots_.size() > 15) 
	{
		oldShots_.erase(oldShots_.begin());
	}
}

void UITankShotHistory::revertSettings(unsigned int index)
{
	if (index < oldShots_.size())
	{
		int newIndex = (int) (oldShots_.size() - 1) - index;
		currentValues_ = oldShots_[newIndex];
		oldValues_ = currentValues_;
	}
	changePower(0);
}

void UITankShotHistory::undo()
{
	currentValues_ = oldValues_;
	changePower(0);
}

fixed UITankShotHistory::getRotationXYDiff()
{
	fixed rotDiff = (fixed(360) - currentValues_.rot) - (fixed(360) - oldValues_.rot);
	if (rotDiff > 180) rotDiff -= 360;
	else if (rotDiff < -180) rotDiff += 360;
	return rotDiff;
}

fixed UITankShotHistory::getRotationYZDiff()
{
	return currentValues_.ele - oldValues_.ele;
}

fixed UITankShotHistory::getPowerDiff()
{
	return currentValues_.power - oldValues_.power;
}

fixed UITankShotHistory::rotateGunXY(fixed angle, bool diff)
{
	if (diff) currentValues_.rot += angle;
	else currentValues_.rot = angle;

	if (currentValues_.rot <= 0) currentValues_.rot = currentValues_.rot + 360;
	else if (currentValues_.rot > 360) currentValues_.rot = currentValues_.rot - 360;

	return currentValues_.rot;
}

fixed UITankShotHistory::rotateGunYZ(fixed angle, bool diff)
{
	if (diff) currentValues_.ele += angle;
	else currentValues_.ele = angle;

	if (currentValues_.ele < 0) currentValues_.ele = 0;
	else if (currentValues_.ele > 90) currentValues_.ele = 90;

	return currentValues_.ele;
}

fixed UITankShotHistory::changePower(fixed power, bool diff)
{
	if (diff) currentValues_.power += power;
	else currentValues_.power = power;

	if (currentValues_.power < 0) currentValues_.power = 0;
	if (currentValues_.power > maxPower_) currentValues_.power = maxPower_;

	return currentValues_.power;
}

void UITankShotHistory::setMaxPower(fixed maxPower) 
{ 
	maxPower_ = maxPower; 
	changePower(0);
}

std::string UITankShotHistory::getRotationString()
{
	char messageBuffer[255];
	fixed rotDiff = getRotationXYDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 
		(fixed(360) - currentValues_.rot).asFloat(),
		rotDiff.asFloat());
	return messageBuffer;
}

std::string UITankShotHistory::getElevationString()
{
	char messageBuffer[255];
	fixed rotDiff = getRotationYZDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 
		currentValues_.ele.asFloat(),
		rotDiff.asFloat());
	return messageBuffer;
}

std::string UITankShotHistory::getPowerString()
{
	char messageBuffer[255];
	fixed powDiff = getPowerDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 		
		currentValues_.power.asFloat(),
		powDiff.asFloat());
	return messageBuffer;
}
