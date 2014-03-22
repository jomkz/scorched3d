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

#include <scorched3dc/DocumentGenerator.h>
#include <events/EventHandlerDataBase.h>
#include <landscapedef/LandscapeDescriptionsBase.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeInclude.h>
#include <common/OptionsMasterListServer.h>
#include <common/OptionsGame.h>
#include <engine/ModInfo.h>
#include <weapons/Accessory.h>
#include <client/UniqueIdStore.h>
#include <client/ClientOptions.h>
#include <tankai/TankAICurrent.h>
#include <tankai/TankAIWeaponSets.h>
#include <tanket/TanketTypes.h>
#include <tank/TankModelStore.h>
#include <models/ModelStore.h>
#include <template/TemplateRenderer.h>
#include <XML/XMLEntryDocumentGenerator.h>

void DocumentGenerator::generatDocumentation(const std::string &directory)
{
	/* TEST CODE
	TemplateRenderer renderer;

	TemplateProviderLocal local(0);
	local.addLocalVariable("aa", new TemplateProviderString("AAVALUE"));
	TemplateProviderLocal *valid = new TemplateProviderLocal(&local);
	local.addLocalVariable("valid", valid);
	valid->addLocalVariable("var", new TemplateProviderString("CHILDVALUE"));

	std::list<TemplateProvider *> lst;
	lst.push_back(new TemplateProviderString("A"));
	lst.push_back(new TemplateProviderString("B"));
	lst.push_back(new TemplateProviderString("C"));
	local.addLocalVariable("listvar", new TemplateProviderList(lst));

	TemplateData data;
	renderer.renderTemplateToFile(data, &local, "docs/text.txt", 
		S3D::formatStringBuffer("%s/test.txt", directory.c_str()));
	*/

	XMLEntryDocumentGenerator documentGenerator(directory);
	OptionsGame optionsGame;
	documentGenerator.addXMLEntryRoot(&optionsGame);

	LandscapeTextureFile landscapeTex;
	documentGenerator.addXMLEntryRoot(&landscapeTex);

	LandscapeDefinitionFile landscapeDefn;
	documentGenerator.addXMLEntryRoot(&landscapeDefn);

	LandscapeIncludeFile landscapeInclude;
	documentGenerator.addXMLEntryRoot(&landscapeInclude);

	LandscapeDescriptionsBase landscapeDescriptions;
	documentGenerator.addXMLEntryRoot(&landscapeDescriptions);

	AccessoryRoot accessory;
	documentGenerator.addXMLEntryRoot(&accessory);

	UniqueIdStore::EntryRoot uniqueIdStore;
	documentGenerator.addXMLEntryRoot(&uniqueIdStore);

	documentGenerator.addXMLEntryRoot(ClientOptions::instance());
	documentGenerator.addXMLEntryRoot(OptionsMasterListServer::instance());
	
	ModInfoRoot modInfo;
	documentGenerator.addXMLEntryRoot(&modInfo);

	DataBaseSettings databaseSettings;
	documentGenerator.addXMLEntryRoot(&databaseSettings);

	TankAICurrentFile tankAICurrent;
	documentGenerator.addXMLEntryRoot(&tankAICurrent);
	TankAIWeaponSets weaponSets;
	documentGenerator.addXMLEntryRoot(&weaponSets);

	TanketTypes tanketTypes;
	documentGenerator.addXMLEntryRoot(&tanketTypes);

	TankModelStore tankModelStore;
	documentGenerator.addXMLEntryRoot(&tankModelStore);

	ModelStore modelStore;
	documentGenerator.addXMLEntryRoot(&modelStore);

	documentGenerator.writeDocumentation();
}
