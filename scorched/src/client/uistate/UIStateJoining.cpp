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
#include <uistate/UIStateJoining.h>
#include <dialogs/GUIProgressCounter.h>
#include <engine/ThreadCallback.h>
#include <client/ScorchedClient.h>
#include <uiactions/UIJoinGameAction.h>

UIStateJoining::UIStateJoining() : UIStateI(UIState::StateJoining)
{
}

UIStateJoining::~UIStateJoining()
{
}

void UIStateJoining::createState()
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	{
		CEGUI::Window *join = wmgr.createWindow("OgreTray/Button", "JoinButton");
		join->setText("Join");
		join->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0.0f), CEGUI::UDim(0.00f, 0.0f)));
		join->setSize(CEGUI::USize(CEGUI::UDim(0.15f, 0.0f), CEGUI::UDim(0.05f, 0.0f)));
		join->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&UIStateJoining::join, this));

		CEGUI::Window *root = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
		root->addChild(join);
	}
}

bool UIStateJoining::join(const CEGUI::EventArgs &e)
{
	ScorchedUI::instance()->getUIState().setState(UIState::StateProgress);
	GUIProgressCounter::instance()->setNewOp(LANG_STRING("Waiting for server..."));
	ScorchedClient::getClientUISyncExternal().addActionFromUI(new UIJoinGameAction());
	return true;
}

void UIStateJoining::destroyState()
{
	CEGUI::Window *root = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.destroyWindow(root->getChild("JoinButton"));
}
