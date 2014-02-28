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

#include <weapons/WeaponInvokeWeapon.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponInvokeWeapon);

WeaponInvokeWeapon::WeaponInvokeWeapon() :
	Weapon("WeaponInvokeWeapon",
		"This primitive calls another pre-defined weapon 'X'. "
		"If a person is killed by this weapon it would say killed by weapon X. "
		"The kill money etc would be calculated from X's attributes."),
	invoke_("Name of the accessory to give.  The accessory being invoked MUST BE DEFINED BEFORE this one.")
{
	addChildXMLEntry("invoke", &invoke_);
}

WeaponInvokeWeapon::~WeaponInvokeWeapon()
{
}

bool WeaponInvokeWeapon::readXML(XMLNode *node, void *xmlData)
{
	if (!Weapon::readXML(node, xmlData)) return false;

	AccessoryCreateContext *context = (AccessoryCreateContext *) xmlData;
	Accessory *accessory = context->getAccessoryStore().
		findByPrimaryAccessoryName(invoke_.getValue().c_str());
	if (!accessory)
	{
		return node->returnError(
			S3D::formatStringBuffer("Failed to find accessory named %s",
			invoke_.getValue().c_str()));
	}

	AccessoryPart *accessoryPart = accessory->getAction();
	if (!accessoryPart || accessoryPart->getType() != AccessoryPart::AccessoryWeapon)
	{
		return node->returnError("Failed to find sub weapon, not a weapon");
	}
	invokeWeapon_ = (Weapon*) accessoryPart;

	return true;
}

void WeaponInvokeWeapon::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	std::set<Weapon *> &weaponStack = weaponContext.getInternalContext().getWeaponStack();
	if (weaponStack.find(invokeWeapon_) != weaponStack.end())
	{
		S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
			"WeaponInvokeWeapon recursive called for weapon %s",
			invokeWeapon_->getParent()->getName()));
	}
    
	weaponStack.insert(invokeWeapon_);
	invokeWeapon_->fire(context, weaponContext, position, velocity);
	weaponStack.erase(invokeWeapon_);
}
