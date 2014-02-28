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

#include <weapons/WeaponLightning.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <actions/Lightning.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponLightning);

WeaponLightning::WeaponLightning() :
	Weapon("WeaponLightning", "Creates a bolt of lightning with the specified properties"),
	coneLengthExp_("WeaponLightning::coneLengthExp", "Length of the lightning bolt"),
	segLengthExp_("WeaponLightning::segLengthExp", "Length of each segment of the bolt"),
	segVarExp_("WeaponLightning::segVarExp", "Max length variance of each segment"),
	sizeExp_("WeaponLightning::sizeExp", "Width of the lightning segments"),
	sizeVarExp_("WeaponLightning::sizeVarExp", "The amount added to the width for each new segment.  Segments get larger each itteration"),
	minSizeExp_("WeaponLightning::minSizeExp", "Minimum width of a segment"),
	splitProbExp_("WeaponLightning::splitProbExp", "Probability that the bolt will fork into multiple segments"),
	splitVarExp_("WeaponLightning::splitVarExp", "The chance the lightning will split as it gets longer.  The chance gets larger the longer it is and this increase is controlled by this item "),
	deathProbExp_("WeaponLightning::deathProbExp", "Probability that the bolt will stop at a given segment"),
	derivAngleExp_("WeaponLightning::derivAngleExp", "The minimum allowed angle change between the very first segment and current segment's direction"),
	angleVarExp_("WeaponLightning::angleVarExp", "The maximum angle change that will be tried for the next segment"),
	totalTimeExp_("WeaponLightning::totalTimeExp", "Total life time of the lightning particles, in seconds"),
	segHurtExp_("WeaponLightning::segHurtExp", "Amount of damage done to tanks per segment"),
	segHurtRadiusExp_("WeaponLightning::segHurtRadiusExp", "Radius within which segments will do damage"),
	texture_("Location of the texture to apply to the lightning effect")
{
	addChildXMLEntry("conelength", &coneLengthExp_);
	addChildXMLEntry("seglength", &segLengthExp_);
	addChildXMLEntry("segvar", &segVarExp_);
	addChildXMLEntry("size", &sizeExp_);
	addChildXMLEntry("sizevar", &sizeVarExp_);
	addChildXMLEntry("minsize", &minSizeExp_);
	addChildXMLEntry("splitprob", &splitProbExp_);
	addChildXMLEntry("splitvar", &splitVarExp_);
	addChildXMLEntry("deathprob", &deathProbExp_);
	addChildXMLEntry("derivangle", &derivAngleExp_);
	addChildXMLEntry("anglevar", &angleVarExp_);
	addChildXMLEntry("totaltime", &totalTimeExp_);
	addChildXMLEntry("seghurt", &segHurtExp_);
	addChildXMLEntry("seghurtradius", &segHurtRadiusExp_);
	addChildXMLEntry("texture", &texture_);
}

WeaponLightning::~WeaponLightning()
{
}

void WeaponLightning::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	coneLength_ = coneLengthExp_.getValue(context);
	segLength_ = segLengthExp_.getValue(context);
	segVar_ = segVarExp_.getValue(context);
	size_ = sizeExp_.getValue(context);
	sizeVar_ = sizeVarExp_.getValue(context);
	minSize_ = minSizeExp_.getValue(context);
	splitProb_ = splitProbExp_.getValue(context);
	splitVar_ = splitVarExp_.getValue(context);
	deathProb_ = deathProbExp_.getValue(context);
	derivAngle_ = derivAngleExp_.getValue(context);
	angleVar_ = angleVarExp_.getValue(context);
	totalTime_ = totalTimeExp_.getValue(context);
	segHurt_ = segHurtExp_.getValue(context);
	segHurtRadius_ = segHurtRadiusExp_.getValue(context);

	Action *action = new Lightning(
		this, weaponContext, position, velocity); 
	context.getActionController().addAction(action);	
}

