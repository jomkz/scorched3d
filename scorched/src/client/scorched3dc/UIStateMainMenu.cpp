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

#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/UIStateMainMenu.h>
#include <scorched3dc/UIProgressCounter.h>
#include <client/ClientState.h>
#include <client/ClientParams.h>
#include <client/ScorchedClient.h>

UIStateMainMenu::UIStateMainMenu() : UIStateI(UIState::StateMainMenu)
{
}

UIStateMainMenu::~UIStateMainMenu()
{
}

void UIStateMainMenu::createState()
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

	{
		CEGUI::Window *start = wmgr.createWindow("OgreTray/Button", "CEGUIDemo/StartButton");
		start->setText("Start");
		start->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15f, 0.0f), CEGUI::UDim(0.00f, 0.0f)));
		start->setSize(CEGUI::USize(CEGUI::UDim(0.15f, 0.0f), CEGUI::UDim(0.05f, 0.0f)));
		start->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&UIStateMainMenu::start, this));
		sheet->addChild(start);
	}

	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
}

bool UIStateMainMenu::start(const CEGUI::EventArgs &e)
{
	ScorchedUI::instance()->getUIState().setState(UIState::StateProgress);
	ClientParams::instance()->setStartCustom(true);
	ScorchedClient::startClient(UIProgressCounter::instance());
	return true;
}

void UIStateMainMenu::destroyState()
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.destroyAllWindows();
}
