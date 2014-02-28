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

#include <weapons/WeaponGotoLabel.h>
#include <weapons/AccessoryStore.h>

REGISTER_ACCESSORY_SOURCE(WeaponGotoLabel);

WeaponGotoLabel::WeaponGotoLabel() :
	Weapon("WeaponGotoLabel", 
		"Loops back to a place in the XML defined by a WeaponLabel tag. This can be used to make chains of translates and other hard to repeat actions."),
	count_("Number of times to loop back"),
	label_("Label name of spot in code to loop back to")
{
	addChildXMLEntry("label", &label_);
	addChildXMLEntry("count", &count_);
}

WeaponGotoLabel::~WeaponGotoLabel()
{
}

bool WeaponGotoLabel::readXML(XMLNode *node, void *xmlData)
{
	if (!Weapon::readXML(node, xmlData)) return false;

	AccessoryCreateContext *context = (AccessoryCreateContext *) xmlData;
	weaponLabel_ = context->getLabel(label_.getValue().c_str());
	if (!weaponLabel_)
	{
		return node->returnError("Failed to find the named label");
	}

	return true;
}

void WeaponGotoLabel::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	int doneCount = weaponContext.getInternalContext().getIncLabelCount(getAccessoryPartId());
	if (doneCount <= count_.getValue())
	{
		weaponLabel_->fire(context, weaponContext, position, velocity);
	}
}
