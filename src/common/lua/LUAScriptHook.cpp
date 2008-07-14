////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <lua/LUAScriptHook.h>
#include <XML/XMLFile.h>

LUAScriptHook::LUAScriptHook(LUAScriptFactory *factory) :
	factory_(factory)
{
}

LUAScriptHook::~LUAScriptHook()
{
}

void LUAScriptHook::addHookProvider(const std::string &hookName)
{
	hookNames_.insert(
		std::pair<std::string, std::vector<HookEntry> >
			(hookName, std::vector<HookEntry>()));
}

void LUAScriptHook::callHook(const std::string &hookName, std::vector<Param> &params)
{
	std::map<std::string, std::vector<HookEntry> >::iterator hookItor =
		hookNames_.find(hookName);
	if (hookItor == hookNames_.end()) return;

	std::vector<Param>::iterator paramItor, 
		endParamItor = params.end();
	std::vector<HookEntry>::iterator 
		itor = hookItor->second.begin(), 
		endItor = hookItor->second.end();
	for (;itor != endItor;itor++)
	{
		LUAScript *script = itor->script;
		if (!script->startFunction(itor->entryPoint)) continue;

		for (paramItor = params.begin();
			paramItor != endParamItor;
			paramItor++)
		{
			if (paramItor->type == Param::eNumber)
			{
				script->addNumberParameter(paramItor->number);
			}
			else
			{
				script->addStringParameter(paramItor->str);
			}
		}

		script->endFunction((int) params.size());
	}
}

bool LUAScriptHook::loadHooks(const std::string &fileName)
{
	XMLFile file;
    if (!file.readFile(fileName))
	{
		S3D::dialogMessage("LUAScriptHook", S3D::formatStringBuffer(
			"Failed to parse \"%s\"\n%s", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}
	if (!file.getRootNode()) return true;
	
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 childrenItor++)
    {
        XMLNode *currentNode = (*childrenItor);

		std::string scriptFileName, error;
		if (!currentNode->getNamedChild("filename", scriptFileName)) return false;

		LUAScript *script = factory_->createScript();
		if (!script->loadFromFile(S3D::getDataFile(scriptFileName), error))
		{
			S3D::dialogMessage("LUAScriptHook", S3D::formatStringBuffer(
				"From file %s, failed to parse LUA script \"%s\"\n%s", 
				fileName.c_str(),
				scriptFileName.c_str(),
				error.c_str()));
			return false;
		}

		XMLNode *hook = 0;
		while (currentNode->getNamedChild("hook", hook, false))
		{
			std::string hookName, entryPoint;
			if (!hook->getNamedChild("hookname", hookName)) return false;
			if (!hook->getNamedChild("entrypoint", entryPoint)) return false;

			std::map<std::string, std::vector<HookEntry> >::iterator hookItor =
				hookNames_.find(hookName);
			if (hookItor == hookNames_.end())
			{
				S3D::dialogMessage("LUAScriptHook", S3D::formatStringBuffer(
					"From file %s, failed to find hook %s for LUA script \"%s\"", 
					fileName.c_str(),
					hookName.c_str(),
					scriptFileName.c_str()));
				return false;
			}

			if (!script->functionExists(entryPoint))
			{
				S3D::dialogMessage("LUAScriptHook", S3D::formatStringBuffer(
					"From file %s, failed to find entrypoint %s in LUA script \"%s\"", 
					fileName.c_str(),
					entryPoint.c_str(),
					scriptFileName.c_str()));
				return false;
			}

			HookEntry hookEntry = { script, entryPoint };
			hookItor->second.push_back(hookEntry);
		}
	}

	return true;
}
