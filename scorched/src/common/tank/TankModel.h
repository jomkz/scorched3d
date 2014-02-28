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

#if !defined(__INCLUDE_TankModelh_INCLUDE__)
#define __INCLUDE_TankModelh_INCLUDE__

#include <XML/XMLEntryComplexTypes.h>
#include <XML/XMLEntryRoot.h>

class TankModelTypeList : public XMLEntryList<XMLEntryString>
{
public:
	TankModelTypeList();
	virtual ~TankModelTypeList();

	virtual XMLEntryString *createXMLEntry(void *xmlData);
};

class TankModelCatagoryList : public XMLEntryList<XMLEntryString>
{
public:
	TankModelCatagoryList();
	virtual ~TankModelCatagoryList();

	virtual XMLEntryString *createXMLEntry(void *xmlData);
};

class TankModelTeamList : public XMLEntryList<XMLEntryInt>
{
public:
	TankModelTeamList();
	virtual ~TankModelTeamList();

	virtual XMLEntryInt *createXMLEntry(void *xmlData);
};

class ScorchedContext;
class Tank;
class TankModel : public XMLEntryContainer
{
public:
	TankModel();
	virtual ~TankModel();

	const char *getName() { return modelName_.getValue().c_str(); }
	XMLEntryModel &getTankModel() { return model_; }

	bool isOfCatagory(const char *catagory);
	bool isOfAi(bool ai);
	bool isOfTankType(const char *tankType);
	bool isOfTeam(int team);

	bool availableForTank(Tank *tank);

	std::list<XMLEntryString *> &getCatagories() { return catagories_.getChildren(); }

protected:
	XMLEntryBool aiOnly_;
	XMLEntryString modelName_;
	XMLEntryModel model_;
	TankModelTypeList tankTypes_;
	TankModelCatagoryList catagories_;
	TankModelTeamList teams_;
};

class TankModelList : public XMLEntryList<TankModel>
{
public:
	TankModelList();
	virtual ~TankModelList();

	virtual TankModel *createXMLEntry(void *xmlData);
};

#endif
