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

#include <weapons/Weapon.h>
#include <weapons/AccessoryStore.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <common/OptionsScorched.h>
#include <actions/CameraPositionAction.h>

WeaponFireContextInternal::WeaponFireContextInternal(unsigned int selectPositionX, unsigned int selectPositionY,
		const FixedVector &velocityVector, bool referenced, bool updateStats) :
	referenced_(referenced), updateStats_(updateStats),
	killCount_(0), referenceCount_(0), labelCount_(0),
	velocityVector_(velocityVector),
	selectPositionX_(selectPositionX), selectPositionY_(selectPositionY)
{
}

WeaponFireContextInternal::~WeaponFireContextInternal()
{
	delete labelCount_;
}

void WeaponFireContextInternal::incrementReference()
{
	referenceCount_++;
}

void WeaponFireContextInternal::decrementReference()
{
	referenceCount_--;
	if (referenceCount_ == 0) delete this;
}

int WeaponFireContextInternal::getIncLabelCount(unsigned int label)
{
	if (!labelCount_) labelCount_ = new std::map<unsigned int, int>();

	std::map<unsigned int, int>::iterator findItor =
		labelCount_->find(label);
	if (findItor == labelCount_->end()) (*labelCount_)[label] = 0;

	return ++(*labelCount_)[label];
}

WeaponFireContext::WeaponFireContext(unsigned int playerId, 
	unsigned int selectPositionX, unsigned int selectPositionY,
	const FixedVector &velocityVector,
	bool referenced, bool updateStats) :
	playerId_(playerId)
{
	internalContext_ = new WeaponFireContextInternal(selectPositionX, selectPositionY,
		velocityVector,
		referenced, updateStats);
	internalContext_->incrementReference();
}

WeaponFireContext::WeaponFireContext(WeaponFireContext &other) :
	playerId_(other.playerId_),
	internalContext_(other.internalContext_)
{
	internalContext_->incrementReference();
}

WeaponFireContext::~WeaponFireContext()
{
	internalContext_->decrementReference();
}

WeaponCameraTrack::WeaponCameraTrack() :
	XMLEntryContainer("WeaponCameraTrack",
		"The parameters that define how the \"action\" camera perceive this weapon", 
		false),
	cameraTrack_("Should the action camera track this weapon (when activated)", 0, true),
	showTime_("How long the action camera should focus on this weapon (0 = default)", 0, 0),
	showPriority_("The priority this weapon has over the other weapons (0 = default)", 0, 0)
{
	addChildXMLEntry("cameratrack", &cameraTrack_);
	addChildXMLEntry("showtime", &showTime_);
	addChildXMLEntry("showpriority", &showPriority_);
}

WeaponCameraTrack::~WeaponCameraTrack()
{
}

CameraPositionAction *WeaponCameraTrack::createPositionAction(unsigned int playerId, TankViewPointProvider *provider,
	fixed defaultShowTime, unsigned int defaultPriority,
	bool explosion)
{
	fixed showTime = showTime_.getValue();
	if (showTime == 0) showTime = defaultShowTime;
	unsigned int priority = showPriority_.getValue();
	if (priority == 0) priority = defaultPriority;

	CameraPositionAction *positionAction = new CameraPositionAction(
		playerId, provider,
		showTime, priority, explosion);
	return positionAction;
}

Weapon::Weapon(const char *typeName, const char *description) : 
	AccessoryPart(typeName, description)
{

}

Weapon::~Weapon()
{

}

void Weapon::fire(ScorchedContext &context,
	WeaponFireContext &weaponContext,
	FixedVector &position, FixedVector &velocity)
{
	if (context.getOptionsGame().getWeaponSyncCheck())
	{
		addWeaponSyncCheck(context, weaponContext, position, velocity);
	}

	fireWeapon(context, weaponContext, position, velocity);
}

void Weapon::addWeaponSyncCheck(ScorchedContext &context,
	WeaponFireContext &weaponContext,
	FixedVector &position, FixedVector &velocity)
{
	context.getSimulator().addSyncCheck(S3D::formatStringBuffer("WeaponFire %s-%u-%s %u %s %s",
		getParent()->getName(), getParent()->getAccessoryId(), getAccessoryTypeName(),
		weaponContext.getPlayerId(),
		position.asQuickString(), velocity.asQuickString().c_str()));
}

XMLEntryWeaponChoice::XMLEntryWeaponChoice() :
	XMLEntryTypeChoice<Weapon>("WeaponChoice", "The choice of another weapon")
{
}

XMLEntryWeaponChoice::~XMLEntryWeaponChoice()
{
}

Weapon *XMLEntryWeaponChoice::createXMLEntry(const std::string &type, void *xmlData)
{
	AccessoryPart *part = AccessoryMetaRegistration::getNewAccessory(type.c_str());
	AccessoryCreateContext *context = (AccessoryCreateContext *) xmlData;
	if (context && part)
	{
		DIALOG_ASSERT(part->getType() == AccessoryPart::AccessoryWeapon);
		part->setParent(context->getAccessory());
	}
	return (Weapon *) part;
}

void XMLEntryWeaponChoice::getAllTypes(std::set<std::string> &allTypes)
{
	std::map<std::string, AccessoryPart *>::iterator 
		itor = AccessoryMetaRegistration::weaponMap->begin(),
		end = AccessoryMetaRegistration::weaponMap->end();
	for (;itor!=end;++itor)
	{
		allTypes.insert(itor->first);
	}
}
