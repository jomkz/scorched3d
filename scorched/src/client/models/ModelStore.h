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

#if !defined(AFX_ModelStore_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
#define AFX_ModelStore_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_

#include <XML/XMLEntryComplexTypes.h>
#include <XML/XMLEntryRoot.h>

class ModelStoreEntry : public XMLEntryContainer
{
public:
	ModelStoreEntry();
	virtual ~ModelStoreEntry();

	XMLEntryString modelName;
	XMLEntryModelSpecDefinition modelDefinition;
};

class ModelStoreEntries : public XMLEntryList<ModelStoreEntry>
{
public:
	ModelStoreEntries();
	virtual ~ModelStoreEntries();

	virtual ModelStoreEntry *createXMLEntry(void *xmlData);
	virtual void clear();

	std::map<std::string, ModelStoreEntry *> &getEntries() { return entries_;  }

protected:
	std::map<std::string, ModelStoreEntry *> entries_;
	virtual bool listEntryCreated(ModelStoreEntry *newEntry, XMLNode *node, void *xmlData);
};

class ModelStore : public XMLEntryRoot<XMLEntrySimpleContainer>
{
public:
	ModelStore();
	virtual ~ModelStore();

	XMLEntryModelSpecDefinition *getModelDefinition(const std::string &name);

	bool loadModels();
	void clearModels();
private:
	ModelStoreEntries entries_;
};

#endif // !defined(AFX_ModelStore_H__516D85F7_420B_43EB_B0BE_563DCBE1B143__INCLUDED_)
