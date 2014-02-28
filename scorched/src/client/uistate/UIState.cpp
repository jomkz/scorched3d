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
#include <uistate/UIState.h>
#include <uistate/UIStateI.h>
#include <uistate/UIStateMainMenu.h>
#include <uistate/UIStateProgress.h>
#include <uistate/UIStateJoining.h>
#include <uistate/UIStatePlaying.h>
#include <client/ScorchedClient.h>

UIState::UIState() : currentState_(0)
{
	uiStateMenuMenu_ = new UIStateMainMenu();
	uiStateProgress_ = new UIStateProgress();
	uiStateJoining_ = new UIStateJoining();
	uiStatePlaying_ = new UIStatePlaying();
}

UIState::~UIState()
{
}

void UIState::setState(State nextState)
{
	if (currentState_)
	{
		if (nextState == currentState_->getState()) return;
		currentState_->destroyState();
	}
	switch (nextState)
	{
	case StateMainMenu:
		currentState_ = uiStateMenuMenu_;
		break;
	case StateProgress:
		currentState_ = uiStateProgress_;
		break;
	case StateJoining:
		currentState_ = uiStateJoining_;
		break;
	case StatePlaying:
		currentState_ = uiStatePlaying_;
		break;
	}
	currentState_->createState();
}

UIState::State UIState::getState() 
{ 
	return currentState_->getState(); 
}

void UIState::setStateNonUIThread(State nextState)
{
	ScorchedClient::instance()->getClientUISync().addActionFromClient(
		new UIStateClientUISyncAction(nextState));
}

void UIState::updateState(float frameTime)
{
	currentState_->updateState(frameTime);
}

UIStateClientUISyncAction::UIStateClientUISyncAction(UIState::State state) :
	state_(state)
{
}

UIStateClientUISyncAction::~UIStateClientUISyncAction()
{
}

void UIStateClientUISyncAction::performUIAction()
{
	ScorchedUI::instance()->getUIState().setState(state_);
	delete this;
}
