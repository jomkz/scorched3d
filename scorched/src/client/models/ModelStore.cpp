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

#include <models/ModelStore.h>

ModelStoreEntry::ModelStoreEntry() :
	XMLEntryContainer("ModelStoreEntry", "The definition of a model that can later be used in a reference"),
	modelName("The name of the model, this is the name that this model can be refered to as")
{
	addChildXMLEntry("modelname", &modelName);
	addChildXMLEntry("modeldefinition", &modelDefinition);
}

ModelStoreEntry::~ModelStoreEntry()
{
}

ModelStoreEntries::ModelStoreEntries() :
	XMLEntryList<ModelStoreEntry>("The list of pre-defined model definitions", 0)
{
}

ModelStoreEntries::~ModelStoreEntries()
{
}

void ModelStoreEntries::clear()
{
	XMLEntryList<ModelStoreEntry>::clear();
	entries_.clear();
}

bool ModelStoreEntries::listEntryCreated(ModelStoreEntry *newEntry, XMLNode *node, void *xmlData)
{
	if (entries_.find(newEntry->modelName.getValue()) != entries_.end()) return false;
	entries_[newEntry->modelName.getValue()] = newEntry;
	return true;
}

ModelStoreEntry *ModelStoreEntries::createXMLEntry(void *xmlData)
{
	return new ModelStoreEntry();
}

ModelStore::ModelStore() : 
	XMLEntryRoot<XMLEntrySimpleContainer>(S3D::eModLocation, "data/models.xml", "models", 
		"ModelStore",
		"A collection of pre-defined models that can later be refered to by name (using XMLEntryModelSpecReference)")
{
	addChildXMLEntry("model", &entries_);
}

ModelStore::~ModelStore()
{
	clearModels();
}

bool ModelStore::loadModels()
{
	return loadFile(true, 0);
}

void ModelStore::clearModels()
{
	entries_.clear();
}

XMLEntryModelSpecDefinition *ModelStore::getModelDefinition(const std::string &name)
{
	std::map<std::string, ModelStoreEntry *>::iterator itor = entries_.getEntries().find(name);
	if (itor == entries_.getEntries().end()) return 0;
	return &itor->second->modelDefinition;
}
