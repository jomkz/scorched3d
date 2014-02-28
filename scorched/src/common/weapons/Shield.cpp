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

#include <weapons/Shield.h>
#include <common/VectorLib.h>
#include <common/Defines.h>
#include <math.h>

static XMLEntryEnum::EnumEntry laserProofEnum[] =
{
	{ "LaserProofNone", Shield::ShieldLaserProofNone },
	{ "LaserProofStop", Shield::ShieldLaserProofStop },
	{ "LaserProofTotal", Shield::ShieldLaserProofTotal },
	{ "", -1 }
};

static XMLEntryEnum::EnumEntry shieldMovementEnum[] =
{
	{ "ShieldMovementAll", Shield::ShieldMovementAll },
	{ "ShieldMovementNone", Shield::ShieldMovementNone },
	{ "ShieldMovementTeamSame", Shield::ShieldMovementSame },
	{ "ShieldMovementTeamRed", Shield::ShieldMovementTeamRed },
	{ "ShieldMovementTeamBlue", Shield::ShieldMovementTeamBlue },
	{ "ShieldMovementTeamGreen", Shield::ShieldMovementTeamGreen },
	{ "ShieldMovementTeamYellow", Shield::ShieldMovementTeamYellow },
	{ "", -1 }
};

Shield::Shield(const char *typeName, const char *description) : 
	AccessoryPart(typeName, description),
	removePower_("Base damage done to shield in a projectile collision."
           "This is also adjustable in the projectile by <shieldhurtfactor> in WeaponProjectile"),
	penetration_("How much damage makes it through to the shield"),
	power_("The amount of damage the shield can absorb."),
	laserProof_("If a shield will stop lasers.  Total proofing prevents any damage from the laser should it hit the shield.", 0, ShieldLaserProofNone, laserProofEnum), 
	movementProof_("If a shield will allow others to move (drive) through it", 0, ShieldMovementAll, shieldMovementEnum)
{
	addChildXMLEntry("removepower", &removePower_);
	addChildXMLEntry("penetration", &penetration_);
	addChildXMLEntry("power", &power_);
	addChildXMLEntry("laserproof", &laserProof_);
	addChildXMLEntry("movementproof", &movementProof_);
}

Shield::~Shield()
{
}
