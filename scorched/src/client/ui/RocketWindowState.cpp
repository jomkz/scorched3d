////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <ui/RocketWindowState.h>
#include <ui/AnimatedIslandDecoratorInstancer.h>
#include <ui/BackdropDecoratorInstancer.h>
#include <XML/XMLFile.h>
#include <common/DefinesFile.h>

RocketWindowState::RocketWindowState(Rocket::Core::Context *context) :
	context_(context)
{
}

RocketWindowState::~RocketWindowState()
{
}

void RocketWindowState::initialize()
{
	Rocket::Core::DecoratorInstancer* islandDecorator = new AnimatedIslandDecoratorInstancer();
	Rocket::Core::Factory::RegisterDecoratorInstancer("island", islandDecorator);
	islandDecorator->RemoveReference();

	Rocket::Core::DecoratorInstancer* backdropDecorator = new BackdropDecoratorInstancer();
	Rocket::Core::Factory::RegisterDecoratorInstancer("backdrop", backdropDecorator);
	backdropDecorator->RemoveReference();

	loadDefinitions();
}

bool RocketWindowState::Window::initializeFromXml(XMLNode *node)
{
	document_ = 0;
	if (!node->getNamedParameter("name", name_)) return false;
	if (!node->getNamedChild("file", file_)) return false;
	if (!S3D::fileExists(S3D::getDataFile(file_))) return false;
	return node->failChildren();
}

void RocketWindowState::Window::open(Rocket::Core::Context *context)
{
	if (document_) return;
	std::string demoFile = S3D::getDataFile(file_);
	document_ = context->LoadDocument(demoFile.c_str());
	document_->Show();
}

void RocketWindowState::Window::close(Rocket::Core::Context *context)
{
	if (!document_) return;
	document_->Close();
	document_->RemoveReference();
	document_ = 0;
}

bool RocketWindowState::State::initializeFromXml(XMLNode *node)
{
	if (!node->getNamedParameter("name", name_)) return false;

	XMLNode *window;
	while (node->getNamedChild("window", window, false))
	{
		std::string windowName;
		if (!window->getNamedParameter("name", windowName)) return false;
		windowNames_.insert(windowName);
	}

	return node->failChildren();
}

bool RocketWindowState::loadDefinitions()
{
	std::map<std::string, Window *>::iterator witor;
	for (witor = windows_.begin();
		witor != windows_.end();
		++witor)
	{
		delete witor->second;
	}
	windows_.clear();
	std::map<std::string, State *>::iterator sitor;
	for (sitor = states_.begin();
		sitor != states_.end();
		++sitor)
	{
		delete sitor->second;
	}

	XMLFile file;
	std::string fileName = S3D::getDataFile("data/rocket/windowstate.xml");
	if (!file.readFile(fileName) ||
		!file.getRootNode())
	{
		S3D::dialogMessage("RocketWindowState", S3D::formatStringBuffer(
			"Failed to parse \"%s\"\n%s", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}

	// Itterate all of the windows in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
        childrenItor != children.end();
        ++childrenItor)
    {
		// For each node named window
        XMLNode *currentNode = (*childrenItor);

		if (0 == strcmp(currentNode->getName(), "window"))
		{
			Window *window = new Window();
			if (!window->initializeFromXml(currentNode)) return false;
			windows_[window->getName()] = window;
		}
		else if (0 == strcmp(currentNode->getName(), "state"))
		{
			State *state = new State();
			if (!state->initializeFromXml(currentNode)) return false;
			states_[state->getName()] = state;

			std::set<std::string>::iterator wNameItor;
			for (wNameItor = state->getWindowNames().begin();
				wNameItor != state->getWindowNames().end();
				++wNameItor)
			{
				std::map<std::string, Window *>::iterator windowItor = windows_.find(*wNameItor);
				if (windowItor == windows_.end())
				{
					S3D::dialogMessage("RocketWindowState", S3D::formatStringBuffer(
						"Failed to find a window called \"%s\" in state \"%s\"", 
						wNameItor->c_str(),
						state->getName().c_str()));
					return false;
				}
			}
		}

		if (!currentNode->failChildren()) return false;
	}

	return true;
}

void RocketWindowState::setState(const std::string &stateName)
{
	static std::set<std::string> emptySet;
	std::set<std::string> *currentSet = &emptySet;
	std::map<std::string, State *>::iterator stateItor = states_.find(stateName);
	if (stateItor != states_.end()) 
	{
		State *state = stateItor->second;
		currentSet = &state->getWindowNames();
	}

	std::map<std::string, Window *>::iterator windowItor;
	for (windowItor = windows_.begin();
		windowItor != windows_.end();
		++windowItor)
	{
		Window *window = windowItor->second;
		if (currentSet->find(window->getName()) == currentSet->end())
		{
			window->close(context_);
		}
		else
		{
			window->open(context_);
		}
	}
}
