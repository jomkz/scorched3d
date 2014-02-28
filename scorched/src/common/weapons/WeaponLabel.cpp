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

#include <weapons/WeaponLabel.h>
#include <weapons/AccessoryStore.h>

REGISTER_ACCESSORY_SOURCE(WeaponLabel);

WeaponLabel::WeaponLabel() :
	Weapon("WeaponLabel",
		"Used to tag a spot in the code so that you can loop back to it later on using WeaponGotoLabel."),
	label_("Label's identifier, can be invoked by WeaponGotoLabel")
{
	addChildXMLEntry("label", &label_);
	addChildXMLEntry("nextweapon", &nextWeapon_);
}

WeaponLabel::~WeaponLabel()
{
}

bool WeaponLabel::readXMLEntry(XMLNode *node, void *xmlData, const char *name, XMLEntry *entry)
{
	XMLEntrySimpleType *simple = (XMLEntrySimpleType *) entry; 

	if (0 == strcmp(name, "label"))
	{
		return entry->readXML(node, xmlData);
	}
	else
	{
		if (label_.getValue().empty())
		{
			return node->returnError("ERROR: Empty label, WeaponLabel must define the weapon after the label element"); 
		}

		AccessoryCreateContext *context = (AccessoryCreateContext *) xmlData;
		context->addLabel(label_.getValue().c_str(), this);
		bool result = entry->readXML(node, xmlData);
		context->removeLabel(label_.getValue().c_str());
		return result;
	}
}

void WeaponLabel::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	nextWeapon_.getValue()->fire(context, weaponContext, position, velocity);
}
