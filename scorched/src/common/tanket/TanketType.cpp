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

#include <tanket/TanketType.h>
#include <XML/XMLParser.h>
#include <common/ToolTip.h>
#include <common/Defines.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>

TanketTypeStartingAccessory::TanketTypeStartingAccessory() :
	XMLEntryContainer("TanketTypeStartingAccessory", 
		"An accessory that this tank type will have at the start of the game without needing to buy it."
		"Starting number of accessories can also be defined in the accessories file, "
		"however this file specifies this on a per tank type basis"),
	name("The name of the accessory as defined in the accessories file"),
	count("The number of accessories that this tank should start with (-1 is an infinite number)")
{
	addChildXMLEntry("name", &name);
	addChildXMLEntry("count", &count);
}

TanketTypeStartingAccessory::~TanketTypeStartingAccessory()
{
}

TanketTypeStartingAccessoryList::TanketTypeStartingAccessoryList() :
	XMLEntryList<TanketTypeStartingAccessory>("All the accessories that this tank will start with", 0)
{
}

TanketTypeStartingAccessoryList::~TanketTypeStartingAccessoryList()
{
}

TanketTypeStartingAccessory *TanketTypeStartingAccessoryList::createXMLEntry(void *xmlData)
{
	return new TanketTypeStartingAccessory();
}

TanketTypeDisabledAccessory::TanketTypeDisabledAccessory() :
	XMLEntryContainer("TanketTypeDisabledAccessory", 
		"An accessory that this tank type will not have access to (even if the tank picks it up via a power up)."),
	name("The name of the accessory as defined in the accessories file")
{
	addChildXMLEntry("name", &name);
}

TanketTypeDisabledAccessory::~TanketTypeDisabledAccessory()
{
}

TanketTypeDisabledAccessoryList::TanketTypeDisabledAccessoryList() :
	XMLEntryList<TanketTypeDisabledAccessory>("All the accessories that this tank will start with", 0)
{
}

TanketTypeDisabledAccessoryList::~TanketTypeDisabledAccessoryList()
{
}

TanketTypeDisabledAccessory *TanketTypeDisabledAccessoryList::createXMLEntry(void *xmlData)
{
	return new TanketTypeDisabledAccessory();
}

TanketType::TanketType() : 
	XMLEntryContainer("TanketType", "Defines a new type of a tank.  "
		"All tanks have an associated type, this type defines the base characteristics of the tank"),
	name_("The name of this tank type"),
	description_("The description of this tank type"),
	life_("The life (health) that this tank will spawn with, also defines the maximum health that this tank can have (excluding powerups)."),
	power_("The power that this tank will spawn with, also defines the maximum power that this tank can have (excluding powerups)."),
	default_("Is this the default tank type", 0, false),
	tooltip_(0)
{
	addChildXMLEntry("name", &name_);
	addChildXMLEntry("description", &description_);
	addChildXMLEntry("default", &default_);
	addChildXMLEntry("life", &life_);
	addChildXMLEntry("power", &power_);
	addChildXMLEntry("accessory", &startingAccessoryList_);
	addChildXMLEntry("disableaccessory", &disabledAccessoryList_);
}

TanketType::~TanketType()
{
}

bool TanketType::readXML(XMLNode *node, void *xmlData)
{
	if (!XMLEntryContainer::readXML(node, xmlData)) return false;

	ScorchedContext *context = (ScorchedContext *) xmlData;
	{
		std::list<TanketTypeStartingAccessory *>::iterator 
			itor = startingAccessoryList_.getChildren().begin(),
			end = startingAccessoryList_.getChildren().end();
		for (;itor!=end;++itor)
		{
			Accessory *accessory = context->getAccessoryStore().
				findByPrimaryAccessoryName((*itor)->name.getValue().c_str());
			if (!accessory)
			{
				return node->returnError(
					S3D::formatStringBuffer("Failed to find named starting accessory %s",
						(*itor)->name.getValue().c_str()));
			}

			accessories_[accessory] = (*itor)->count.getValue();
		}
	}
	{
		std::list<TanketTypeDisabledAccessory *>::iterator 
			itor = disabledAccessoryList_.getChildren().begin(),
			end = disabledAccessoryList_.getChildren().end();
		for (;itor!=end;++itor)
		{
			Accessory *accessory = context->getAccessoryStore().
				findByPrimaryAccessoryName((*itor)->name.getValue().c_str());
			if (!accessory)
			{
				return node->returnError(
					S3D::formatStringBuffer("Failed to find named disabled accessory %s",
						(*itor)->name.getValue().c_str()));
			}
			disabledAccessories_.insert(accessory);
		}
	}

	formTooltip();
	return true;
}

bool TanketType::getAccessoryDisabled(Accessory *accessory)
{
	if (disabledAccessories_.empty()) return false;
	return (disabledAccessories_.find(accessory) != disabledAccessories_.end());
}

void TanketType::formTooltip()
{
	delete tooltip_;

	std::string accessoryBuffer;
	{
		if (!accessories_.empty()) accessoryBuffer.append("\n");

		unsigned int count = 0;
		std::map<Accessory *, int>::iterator itor;
		for (itor = accessories_.begin();
			itor != accessories_.end();
			++itor, count++)
		{
			Accessory *accessory = (*itor).first;
			accessoryBuffer.append("+ ").append(accessory->getName());
			if (count + 1 < accessories_.size()) accessoryBuffer.append("\n");
		}
	}
	{
		if (!disabledAccessories_.empty()) accessoryBuffer.append("\n");

		unsigned int count = 0;
		std::set<Accessory *>::iterator itor;
		for (itor = disabledAccessories_.begin();
			itor != disabledAccessories_.end();
			++itor, count++)
		{
			Accessory *accessory = (*itor);
			accessoryBuffer.append("- ").append(accessory->getName());
			if (count + 1 < disabledAccessories_.size()) accessoryBuffer.append("\n");
		}
	}

	tooltip_ = new ToolTip(
		ToolTip::ToolTipNone,
		LANG_STRING(name_.getValue()),
		LANG_STRING(S3D::formatStringBuffer(
			"%s\n"
			"Life : %.0f\n"
			"Power : %.0f%s",
			description_.getValue().c_str(),
			getLife().asFloat(),
			getPower().asFloat(),
			accessoryBuffer.c_str())));
}

TanketTypeList::TanketTypeList() :
	XMLEntryList<TanketType>("The list of available tank types", 1)
{
}

TanketTypeList::~TanketTypeList()
{
}

TanketType *TanketTypeList::createXMLEntry(void *xmlData)
{
	return new TanketType();
}
