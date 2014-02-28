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

#include <scorched3dc/GUIConsole.h>
#include <client/ScorchedClient.h>
#include <console/Console.h>
#include <engine/ThreadCallback.h>

GUIConsoleClientThreadCallback::GUIConsoleClientThreadCallback(const CEGUI::String &inMsg) :
	inMsg_(inMsg)
{

}

GUIConsoleClientThreadCallback::~GUIConsoleClientThreadCallback()
{

}

void GUIConsoleClientThreadCallback::callbackInvoked()
{
	ScorchedClient::instance()->getConsole().addLine(true, inMsg_);
	delete this;
}

GUIConsole *GUIConsole::instance()
{
	static GUIConsole instance_;
	return &instance_;
}

GUIConsole::GUIConsole() : consoleWindow_(0)
{
   create();
   setVisible(false);
}

GUIConsole::~GUIConsole()
{
	consoleWindow_ = 0;
}

void GUIConsole::create()
{
	CEGUI::WindowManager *pWindowManager = CEGUI::WindowManager::getSingletonPtr();
	consoleWindow_ = pWindowManager->loadLayoutFromFile("GUIConsole.layout");
 
	if (consoleWindow_)
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(consoleWindow_);
		consoleWindow_->getChild("AddButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&GUIConsole::handle_TextSubmitted, this));
 		consoleWindow_->getChild("EditBox")->subscribeEvent(
			CEGUI::Editbox::EventTextAccepted,
			CEGUI::Event::Subscriber(&GUIConsole::handle_TextSubmitted,this));
	}
}

bool GUIConsole::handle_TextSubmitted(const CEGUI::EventArgs &e)
{
	const CEGUI::String &msg = consoleWindow_->getChild("EditBox")->getText();
	consoleWindow_->getChild("EditBox")->setText("");

	GUIConsoleClientThreadCallback *clientCallback = new GUIConsoleClientThreadCallback(msg);
	ScorchedClient::getClientThreadCallback().addCallback(clientCallback);

	return true;
}

void GUIConsole::outputText(const CEGUI::String &inMsg, const CEGUI::Colour &colour)
{ 
	CEGUI::Listbox *outputWindow = static_cast<CEGUI::Listbox*>(consoleWindow_->getChild("DisplayLines"));
	CEGUI::ListboxTextItem *newItem = new CEGUI::ListboxTextItem(inMsg);
	newItem->setTextColours(colour); 
	outputWindow->ensureItemIsVisible(outputWindow->getItemCount());

	//, CEGUI::HTF_WORDWRAP_LEFT_ALIGNED);
	outputWindow->addItem(newItem); 
}

void GUIConsole::setVisible(bool visible)
{
    consoleWindow_->setVisible(visible);
 
	CEGUI::Editbox* editBox = static_cast<CEGUI::Editbox*>(consoleWindow_->getChild("EditBox"));
    if(visible) editBox->activate();
    else editBox->deactivate();
}
 
bool GUIConsole::isVisible()
{
    return consoleWindow_->isVisible();
}