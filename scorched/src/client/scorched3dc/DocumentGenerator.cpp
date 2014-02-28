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
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <common/OptionsMasterListServer.h>
#include <common/OptionsGame.h>
#include <engine/ModInfo.h>
#include <weapons/Accessory.h>
#include <client/UniqueIdStore.h>
#include <client/ClientOptions.h>

void DocumentGenerator::generatDocumentation(const std::string &directory)
{
	XMLEntryDocumentGenerator documentGenerator(directory);

	OptionsGame optionsGame;
	optionsGame.generateDocumentation(documentGenerator);
	LandscapeTex landscapeTex(0);
	landscapeTex.generateDocumentation(documentGenerator);
	LandscapeDefn landscapeDefn(0);
	landscapeDefn.generateDocumentation(documentGenerator);

	AccessoryRoot accessory;
	accessory.generateDocumentation(documentGenerator);
	UniqueIdStore::EntryRoot uniqueIdStore;
	uniqueIdStore.generateDocumentation(documentGenerator);

	ClientOptions::instance()->generateDocumentation(documentGenerator);
	OptionsMasterListServer::instance()->generateDocumentation(documentGenerator);
	
	ModInfoRoot modInfo;
	modInfo.generateDocumentation(documentGenerator);

	documentGenerator.writeDocumentation();
}
