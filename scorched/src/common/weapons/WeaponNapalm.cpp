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

#include <weapons/WeaponNapalm.h>
#include <actions/Napalm.h>
#include <common/Defines.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>

REGISTER_ACCESSORY_SOURCE(WeaponNapalm);

WeaponNapalm::WeaponNapalm() :
	Weapon("WeaponNapalm", "Flows and burns downhill over a given time for a given amount of damage."),
	napalmTime_("WeaponNapalm::napalmTime", "Time in seconds to burn and flow down hill"),
	napalmHeight_("WeaponNapalm::napalmHeight", "The height of the flow of napalm (the depth of the flow)"),
	stepTime_("WeaponNapalm::stepTime", "Time in seconds between movements downhill"),
	hurtStepTime_("WeaponNapalm::hurtStepTime", "Damage is calculated every hurtsteptime seconds"),
	hurtPerSecond_("WeaponNapalm::hurtPerSecond", "Amount of damage napalm does per second"),
	landscapeErosion_("WeaponNapalm::landscapeErosion", "How much height will be removed for the napalm landscape erosion", 0, "0"),
	numberOfParticles_("How many napalm particles can be created", 0, 100),
	numberStreams_("Number of napalm streams to create at the start point"),
	effectRadius_("Radius within which the napalm will damage opponents"),
	objectDamage_("If the napalm will damage landscape objects", 0, true),
	allowUnderWater_(" Whether or not this napalm can travel under water"),
	singleFlow_("Use a single flow of napalm or cover the whole downward area", 0, false)
{
	addChildXMLEntry("napalmtime", &napalmTime_);
	addChildXMLEntry("napalmheight", &napalmHeight_);
	addChildXMLEntry("steptime", &stepTime_);
	addChildXMLEntry("hurtsteptime", &hurtStepTime_);
	addChildXMLEntry("hurtpersecond", &hurtPerSecond_);
	addChildXMLEntry("landscapeerosion", &landscapeErosion_);
	addChildXMLEntry("numberstreams", &numberStreams_);
	addChildXMLEntry("effectradius", &effectRadius_);
	addChildXMLEntry("objectdamage", &objectDamage_);
	addChildXMLEntry("allowunderwater", &allowUnderWater_);
	addChildXMLEntry("singleflow", &singleFlow_);
	addChildXMLEntry("cameratrack", &cameraTrack_);
}

WeaponNapalm::~WeaponNapalm()
{

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

	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	for (int i=0; i<numberStreams_.getValue(); i++)
	{
		int x = (position[0] + random.getRandFixed("WeaponNapalm") * 4 - 2).asInt();
		int y = (position[1] + random.getRandFixed("WeaponNapalm") * 4 - 2).asInt();
		context.getActionController().addAction(
			new Napalm(x, y, this, weaponContext));
	}
}
