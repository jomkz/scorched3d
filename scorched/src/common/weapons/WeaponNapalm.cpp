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

#include <weapons/WeaponNapalm.h>
#include <actions/Napalm.h>
#include <common/Defines.h>
#ifndef S3D_SERVER
	#include <sound/SoundUtils.h>
#endif
#include <engine/ActionController.h>
#include <landscapemap/LandscapeMaps.h>

REGISTER_ACCESSORY_SOURCE(WeaponNapalm);

WeaponNapalm::WeaponNapalm()
{

}

WeaponNapalm::~WeaponNapalm()
{

}

bool WeaponNapalm::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("napalmtime", napalmTime_)) return false;
	if (!accessoryNode->getNamedChild("napalmheight", napalmHeight_)) return false;
	if (!accessoryNode->getNamedChild("steptime", stepTime_)) return false;
	if (!accessoryNode->getNamedChild("hurtsteptime", hurtStepTime_)) return false;
	if (!accessoryNode->getNamedChild("hurtpersecond", hurtPerSecond_)) return false;
	if (!accessoryNode->getNamedChild("numberstreams", numberStreams_)) return false;
	if (!accessoryNode->getNamedChild("napalmsound", napalmSound_)) return false;
	if (!S3D::checkDataFile(S3D::formatStringBuffer("data/wav/%s", napalmSound_.c_str()))) return false;

	if (!params_.parseXML(accessoryNode)) return false;

	return true;
}

void WeaponNapalm::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	fixed minHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
		position[0], position[1]);

	// Make sure position is not underground
	if (position[2] < minHeight)
	{
		if (minHeight - position[2] > 10) // Give room for shields as well
		{
			return;
		}
	}

	RandomGenerator &random = context.getActionController().getRandom();
	for (int i=0; i<numberStreams_; i++)
	{
		int x = (position[0] + random.getRandFixed() * 4 - 2).asInt();
		int y = (position[1] + random.getRandFixed() * 4 - 2).asInt();
		addNapalm(context, weaponContext, x, y);
	}

#ifndef S3D_SERVER
	if (!context.getServerMode()) 
	{
		if (napalmSound_.c_str()[0] &&
			0 != strcmp(napalmSound_.c_str(), "none"))
		{
			SoundBuffer *expSound = 
				Sound::instance()->fetchOrCreateBuffer(
					S3D::getModFile(S3D::formatStringBuffer("data/wav/%s", napalmSound_.c_str())));
			SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
				expSound, position.asVector());
		}
	}
#endif
}

void WeaponNapalm::addNapalm(ScorchedContext &context, 
	WeaponFireContext &weaponContext, int x, int y)
{
	NapalmParams *params = new NapalmParams(params_);
	params->setNapalmTime(napalmTime_.getValue(context));
	params->setNapalmHeight(napalmHeight_.getValue(context));
	params->setStepTime(stepTime_.getValue(context));
	params->setHurtStepTime(hurtStepTime_.getValue(context));
	params->setHurtPerSecond(hurtPerSecond_.getValue(context));

	// Ensure that the napalm has not hit the walls
	// or anything outside the landscape
	context.getActionController().addAction(
		new Napalm(x, y, this, params, weaponContext));
}
