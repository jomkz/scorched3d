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

#include <weapons/WeaponExplosion.h>
#include <weapons/Accessory.h>
#include <actions/Explosion.h>
#include <engine/ActionController.h>
#include <common/Defines.h>

static XMLEntryEnum::EnumEntry deformTypeEnum[] =
{
	{ "up", WeaponExplosion::DeformUp },
	{ "down", WeaponExplosion::DeformDown },
	{ "none", WeaponExplosion::DeformNone },
	{ "", -1 }
};

REGISTER_ACCESSORY_SOURCE(WeaponExplosion);

WeaponExplosion::WeaponExplosion() :
	Weapon("WeaponExplosion", 
		"Creates an explosion of the given size. Explosions can be used for creating or removing earth, damaging tanks, and various other effects."),
	deform_("Determines how the explosion effects the landscape", 0, (int) DeformDown, deformTypeEnum),
	deformSizeExp_("WeaponExplosion::deformSizeExp", "Actual radius of the explosion, in world units"),
	hurtAmountExp_("WeaponExplosion::hurtAmountExp", "How much damage the explosion will do, 1 = 100 pts"),
	explodeUnderGround_("Can the explosion be underground, if not any underground explosions will do nothing", 0, true),
	onlyHurtShield_("Explosion will only do damage to shields", 0, false),
	noCameraTrack_("Do use this explosion for the action camera views", 0, false)
{
	addChildXMLEntry("deform", &deform_);
	addChildXMLEntry("size", &deformSizeExp_);
	addChildXMLEntry("hurtamount", &hurtAmountExp_);
	addChildXMLEntry("explodeunderground", &explodeUnderGround_);
	addChildXMLEntry("onlyhurtshield", &onlyHurtShield_);
	addChildXMLEntry("nocameratrack", &noCameraTrack_);

}

WeaponExplosion::~WeaponExplosion()
{

}

void WeaponExplosion::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	Action *action = new Explosion(
		position, velocity, this, weaponContext);
	context.getActionController().addAction(action);	
}

