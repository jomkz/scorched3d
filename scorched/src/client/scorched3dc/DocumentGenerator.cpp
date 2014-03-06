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

void DocumentGenerator::generatDocumentation(const std::string &directory)
{
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

	renderer.renderTemplateToFile(&local, "test/test.txt", 
		S3D::formatStringBuffer("%s/test.txt", directory.c_str()));

	exit(0);


	XMLEntryDocumentGenerator documentGenerator(directory);

	OptionsGame optionsGame;
	optionsGame.generateDocumentation(documentGenerator);
	LandscapeTextureFile landscapeTex;
	landscapeTex.generateDocumentation(documentGenerator);
	LandscapeDefinitionFile landscapeDefn;
	landscapeDefn.generateDocumentation(documentGenerator);
	LandscapeIncludeFile landscapeInclude;
	landscapeInclude.generateDocumentation(documentGenerator);
	LandscapeDescriptionsBase landscapeDescriptions;
	landscapeDescriptions.generateDocumentation(documentGenerator);

	AccessoryRoot accessory;
	accessory.generateDocumentation(documentGenerator);
	UniqueIdStore::EntryRoot uniqueIdStore;
	uniqueIdStore.generateDocumentation(documentGenerator);

	ClientOptions::instance()->generateDocumentation(documentGenerator);
	OptionsMasterListServer::instance()->generateDocumentation(documentGenerator);
	
	ModInfoRoot modInfo;
	modInfo.generateDocumentation(documentGenerator);

	DataBaseSettings databaseSettings;
	databaseSettings.generateDocumentation(documentGenerator);

	TankAICurrentFile tankAICurrent;
	tankAICurrent.generateDocumentation(documentGenerator);
	TankAIWeaponSets weaponSets;
	weaponSets.generateDocumentation(documentGenerator);

	TanketTypes tanketTypes;
	tanketTypes.generateDocumentation(documentGenerator);

	TankModelStore tankModelStore;
	tankModelStore.generateDocumentation(documentGenerator);

	ModelStore modelStore;
	modelStore.generateDocumentation(documentGenerator);

	documentGenerator.writeDocumentation();
}
