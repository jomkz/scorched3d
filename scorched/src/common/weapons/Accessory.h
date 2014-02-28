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

#if !defined(AFX_ACCESSORY_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
#define AFX_ACCESSORY_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_

#include <XML/XMLEntryComplexTypes.h>
#include <XML/XMLEntryRoot.h>
#include <weapons/AccessoryPart.h>
#include <weapons/AccessoryCreateContext.h>
#include <lang/LangString.h>
#include <string>
#include <map>

class Accessory : public XMLEntryContainer
{
public:
	Accessory(unsigned int accessoryId);
	virtual ~Accessory();

	enum PositionSelectType
	{
		ePositionSelectNone = 0,
		ePositionSelectFuel = 1,
		ePositionSelectGeneric = 2,
		ePositionSelectLimit = 3,
		ePositionSelectFuelLimit = 4
	};

	const char *getActivationSound();
	const char *getName() { return name_.getValue().c_str(); }
	LangString &getStringName();
	const char *getDescription() { return description_.getValue().c_str(); }
	int getPrice() { return price_.getValue(); }
	int getSellPrice() { return sellPrice_; }
	int getOriginalSellPrice() { return originalSellPrice_; }
	int getOriginalPrice() { return originalPrice_; }
	int getFreeMarketLimits() { return freemarketLimits_.getValue(); }
	int getBundle() { return bundle_.getValue(); }
	int getArmsLevel() { return armsLevel_.getValue(); }
	int getMaximumNumber() { return maximumNumber_.getValue(); }
	int getStartingNumber() { return startingNumber_.getValue(); }
	int getUseNumber() { return useNumber_.getValue(); }
	bool getBotOnly() { return botOnly_.getValue(); }
	bool getNoBuy() { return noBuy_.getValue(); }
	PositionSelectType getPositionSelect() { return  (PositionSelectType) positionSelect_.getValue(); }
	int getPositionSelectLimit() { return positionSelectLimit_.getValue(); }

	const char *getIconName() { return iconName_.getValue().c_str(); }
	const char *getGroupName() { return groupName_.getValue().c_str(); }
	const char *getTabGroupName() { return tabGroupName_.getValue().c_str(); }
	AccessoryPart *getAction() { return accessoryAction_.getValue(); }
	XMLEntryModelID &getModel() { return modelId_; }

	AccessoryPart::AccessoryType getType() { return accessoryAction_.getValue()->getType(); }

	void setPrice(int p) { if (p>0) price_.setValue(p); }
	void setSellPrice(int p) { if (p>0) sellPrice_ = p; }

	unsigned int getAccessoryId() { return accessoryId_; }

	// XMLEntryContainer
	virtual bool readXML(XMLNode *accessoryNode, void *xmlData);
protected:
	unsigned int nextAccessoryId_;
	unsigned int accessoryId_;
	LangString stringName_;
	int sellPrice_, originalPrice_, originalSellPrice_;

	XMLEntryAccessoryPartChoice accessoryAction_;
	XMLEntryString name_, description_;
	XMLEntryInt price_, bundle_;
	XMLEntryInt startingNumber_, useNumber_, maximumNumber_;
	XMLEntryBool botOnly_, noBuy_;
	XMLEntryModelID modelId_;
	XMLEntryString iconName_, groupName_, tabGroupName_;
	XMLEntryString activationSound_;
	XMLEntryEnum positionSelect_;
	XMLEntryInt positionSelectLimit_;
	XMLEntryInt armsLevel_;
	XMLEntryInt freemarketLimits_;
};

class AccessoryList : public XMLEntryList<Accessory>
{
public:
	AccessoryList();
	virtual ~AccessoryList();

	virtual Accessory *createXMLEntry(void *xmlData);
private:
	virtual bool listEntryCreated(Accessory *newEntry, XMLNode *node, void *xmlData);
};

class AccessoryRoot : public XMLEntryRoot<XMLEntryContainer>
{
public:
	AccessoryRoot();
	virtual ~AccessoryRoot();

	AccessoryList &getAccessoryList() { return accessoryList_; }
protected:
	AccessoryList accessoryList_;
};

#endif // !defined(AFX_ACCESSORY_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
