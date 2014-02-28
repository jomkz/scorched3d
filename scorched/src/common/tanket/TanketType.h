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

#if !defined(__INCLUDE_TanketTypeh_INCLUDE__)
#define __INCLUDE_TanketTypeh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>

class TanketTypeStartingAccessory : public XMLEntryContainer
{
public:
	TanketTypeStartingAccessory();
	virtual ~TanketTypeStartingAccessory();

	XMLEntryString name;
	XMLEntryInt count;
};

class TanketTypeStartingAccessoryList : public XMLEntryList<TanketTypeStartingAccessory>
{
public:
	TanketTypeStartingAccessoryList();
	virtual ~TanketTypeStartingAccessoryList();

	virtual TanketTypeStartingAccessory *createXMLEntry(void *xmlData);
};

class TanketTypeDisabledAccessory : public XMLEntryContainer
{
public:
	TanketTypeDisabledAccessory();
	virtual ~TanketTypeDisabledAccessory();

	XMLEntryString name;
};

class TanketTypeDisabledAccessoryList : public XMLEntryList<TanketTypeDisabledAccessory>
{
public:
	TanketTypeDisabledAccessoryList();
	virtual ~TanketTypeDisabledAccessoryList();

	virtual TanketTypeDisabledAccessory *createXMLEntry(void *xmlData);
};

class ToolTip;
class XMLNode;
class Accessory;
class ScorchedContext;
class TanketType : public XMLEntryContainer
{
public:
	TanketType();
	virtual ~TanketType();

	bool getUseAsDefault() { return default_.getValue(); }

	const char *getName() { return name_.getValue().c_str(); }
	ToolTip *getTooltip() { return tooltip_; }
	fixed getLife() { return life_.getValue(); }
	fixed getPower() { return power_.getValue(); }

	std::map<Accessory *, int> &getAccessories() { return accessories_; }
	bool getAccessoryDisabled(Accessory *accessory);

	virtual bool readXML(XMLNode *node, void *xmlData);

protected:
	ToolTip *tooltip_;
	XMLEntryString name_, description_;
	XMLEntryBool default_;
	XMLEntryFixed life_;
	XMLEntryFixed power_;
	TanketTypeStartingAccessoryList startingAccessoryList_;
	TanketTypeDisabledAccessoryList disabledAccessoryList_;
	
	std::map<Accessory *, int> accessories_;
	std::set<Accessory *> disabledAccessories_;

	void formTooltip();
};

class TanketTypeList : public XMLEntryList<TanketType>
{
public:
	TanketTypeList();
	virtual ~TanketTypeList();

	virtual TanketType *createXMLEntry(void *xmlData);
};

#endif // __INCLUDE_TanketTypeh_INCLUDE__
