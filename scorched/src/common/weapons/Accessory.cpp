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

#include <common/ToolTip.h>
#include <common/ModelID.h>
#include <weapons/Accessory.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <lang/LangResource.h>
#include <tank/Tank.h>
#include <tank/TankModel.h>
#include <tank/TankModelContainer.h>
#include <stdlib.h>

static XMLEntryEnum::EnumEntry PositionSelectTypeEnum[] =
{
	{ "none", Accessory::ePositionSelectNone },
	{ "fuel", Accessory::ePositionSelectFuel },
	{ "generic", Accessory::ePositionSelectGeneric },
	{ "limit", Accessory::ePositionSelectLimit },
	{ "fuellimit", Accessory::ePositionSelectFuelLimit },
	{ "", -1 }
};

Accessory::Accessory(unsigned int accessoryId) :
	XMLEntryContainer("Accessory", 
		"An accessory that a player can aquire to boost the capabilities of their tank.  Accessories include weapons, shields, etc..."),
	accessoryId_(accessoryId), 
	name_("The name of this accessory, as displayed in the game."), description_("The description of this accessory, as displayed in the game.", 0, ""), 
	price_("The cost of this accessory to buy, this accessory can be bought in multiples defined by the bundle size", 0, 0), 
	bundle_("The number of these accessories that will be purchased for the buying price", 0, 1),
	modelId_(false),
	startingNumber_("The number of these accessories a player will start each new match with", 0, 0),
	useNumber_("The number of this acessory that will be used when a player actives the accessory", 0, 1),
	maximumNumber_("The maximum number of these accessories that a player can hold", 0, 0),
	botOnly_("If this accessory can only be bought by an AI (a bot)", 0, false),
	noBuy_("If this accessory can be bought at all, accessories that cannot be bought can still be given or aquired through pickups", 0, false),
	iconName_("The name of an icon to use when displaying the weapon on the UI", 0, ""),
	groupName_("The name of the group that this accessory will be added to, each group can be used to contain a set of similar accessories e.g. all weapons", 0, ""),
	tabGroupName_("The name of the group that will group this accessory when it is being bought, e.g. defense, shield, etc...", 0, ""),
	activationSound_("The sound that will be played when this accessory is activated", 0, ""),
	positionSelect_("When activating this weapon it doesn't use the current power, rotation and elevation, but instead required clicking on the landscape", 0, (int) ePositionSelectNone, PositionSelectTypeEnum),
	positionSelectLimit_("The extent/limit that this weapon can be moved if the position select type is chosen", 0, 10),
	armsLevel_("The arms level that this accessory is allowed to be bought in, the current arms level is a game option that allows you to limit weapons to certain rounds (or completely)", 0, 9), 
	freemarketLimits_("The limit (as a percentage) that this accessory price can increase/decrease due to the free market economy", 0, 150)
{
	addChildXMLEntry("name", &name_, "description", &description_);
	addChildXMLEntry("cost", &price_, "bundlesize", &bundle_);
	addChildXMLEntry("startingnumber", &startingNumber_, "usenumber", &useNumber_, "maximumnumber", &maximumNumber_);
	addChildXMLEntry("botonly", &botOnly_, "nobuy", &noBuy_);
	addChildXMLEntry("model", &modelId_);
	addChildXMLEntry("icon", &iconName_, "group", &groupName_, "tabgroup", &tabGroupName_);
	addChildXMLEntry("activationsound", &activationSound_);
	addChildXMLEntry("positionselection", &positionSelect_, "positionselectionlimit", &positionSelectLimit_);
	addChildXMLEntry("armslevel", &armsLevel_);
	addChildXMLEntry("freemarketlimits", &freemarketLimits_);
}

Accessory::~Accessory()
{
}

bool Accessory::readXML(XMLNode *accessoryNode, void *xmlData)
{
	if (!XMLEntryContainer::readXML(accessoryNode, xmlData)) return false;

	if (groupName_.getValue().empty())
	{
		// Get the accessory groupname
		switch (accessoryAction_.getValue()->getType())
		{
		case AccessoryPart::AccessoryWeapon:
			groupName_.setValue("weapon");
			break;
		case AccessoryPart::AccessoryParachute:
			groupName_.setValue("parachute");
			break;
		case AccessoryPart::AccessoryShield:
			groupName_.setValue("shield");
			break;
		case AccessoryPart::AccessoryAutoDefense:
			groupName_.setValue("autodefense");
			break;
		case AccessoryPart::AccessoryBattery:
			groupName_.setValue("battery");
			break;
		default:
			groupName_.setValue("none");
			break;
		}
	}

	if (tabGroupName_.getValue().empty())
	{
		// Get the accessory tabgroupname
		if (accessoryAction_.getValue()->getType() == AccessoryPart::AccessoryWeapon)
		{
			tabGroupName_.setValue("weapon");
		}
		else
		{
			tabGroupName_.setValue("defense");
		}
	}

	// Get the maximum number
	AccessoryCreateContext *creationContext = (AccessoryCreateContext *) xmlData;
	if (maximumNumber_.getValue() == 0 && creationContext)
	{
		maximumNumber_.setValue(creationContext->getOptionsGame().getMaxNumberWeapons());
	}
	
	// Setup price
	sellPrice_ = 0;
	if (price_.getValue() > 0 && bundle_.getValue() > 0) sellPrice_ = int((price_.getValue() / bundle_.getValue()) * 0.8f);
	originalPrice_ = price_.getValue();
	originalSellPrice_ = sellPrice_;

	return true;
}

LangString &Accessory::getStringName()
{
	if (stringName_.size() == 0)
	{
		stringName_ = LANG_RESOURCE(getName(), getName());
	}
	return stringName_;
}

const char *Accessory::getActivationSound()
{
	if (!activationSound_.getValue().c_str()[0]) return 0;
	return activationSound_.getValue().c_str();
}
