
//    Scorched3D (c) 2000-2003
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

#include <engine/GameState.h>
#include <engine/GameStateI.h>
#include <engine/GameStateStimulusI.h>
#include <common/Keyboard.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/LoggerI.h>
#include <limits.h>
#include <SDL/SDL.h>

GameStatePerfCounter::GameStatePerfCounter(GameStateI *gameStateI, const char *name) : 
	name_(name), total_(0), gameStateI_(gameStateI)
{
}

GameStatePerfCounter::~GameStatePerfCounter()
{
}

void GameStatePerfCounter::start()
{
	start_ = SDL_GetTicks();
}

void GameStatePerfCounter::end()
{
	unsigned int end = SDL_GetTicks();
	total_ += end - start_;
}

unsigned int GameStatePerfCounter::getTotal()
{ 
	unsigned int lastTotal = total_;
	total_ = 0;
	return lastTotal; 
}

GameState::GameState(const char *name) :
	name_(name),
	fakeMiddleButton_(true),
	currentMouseState_(0),
	pendingStimulus_(UINT_MAX),
	currentState_(UINT_MAX),
	currentEntry_(0),
	currentStateI_(0),
	currentMouseX_(0), currentMouseY_(0),
	mouseDoubleX_(0), mouseDoubleY_(0),
	stateLogging_(false), 
	stateTimeLogging_(0.0f), frameCount_(0)
{
	clearTimers();
}

GameState::~GameState()
{

}

void GameState::clear()
{
	stateList_.clear();
}

void GameState::setFakeMiddleButton(bool fake)
{
	fakeMiddleButton_ = fake;
}

void GameState::mouseWheel(short z)
{
	bool skipRest = false;
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;

		StateIList *currentList = &currentEntry_->subMouseWheelList;
		if (!currentList->empty())
		{
			StateIList::iterator subItor;
			for (subItor = currentList->begin();
				subItor != currentList->end();
				subItor++)
			{
				(*subItor)->mouseWheel(thisState, 
					currentMouseX_, currentMouseY_, 
					(int) z, skipRest);
				if (checkStimulate()) return;
				if (skipRest) break;
			}
		}
	}
}

void GameState::mouseMove(int x, int y)
{
	currentMouseX_ = x;
	currentMouseY_ = y;
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;

		if (MouseButtonMiddle & currentMouseState_)
		{
			int diffX = x - mouseMDragX_; mouseMDragX_ = x;
			int diffY = mouseMDragY_ - y; mouseMDragY_ = y;

			mouseMoveCall(thisState, MouseButtonMiddle, 
						  currentEntry_->subMouseDragMiddleList,
					x, y, diffX, diffY);
		}

		if (fakeMiddleButton_ && (MouseButtonLeft & currentMouseState_ && 
								  MouseButtonRight & currentMouseState_))
		{
			int diffX = x - mouseMDragX_; mouseMDragX_ = x;
			int diffY = mouseMDragY_ - y; mouseMDragY_ = y;

			mouseMoveCall(thisState, MouseButtonMiddle, 
						  currentEntry_->subMouseDragMiddleList,
					x, y, diffX, diffY);
		}
		else
		{
			if (MouseButtonLeft & currentMouseState_)
			{
				int diffX = x - mouseLDragX_; mouseLDragX_ = x;
				int diffY = mouseLDragY_ - y; mouseLDragY_ = y;

				mouseMoveCall(thisState, MouseButtonLeft, 
							  currentEntry_->subMouseDragLeftList,
						x, y, diffX, diffY);
			}
			if (MouseButtonRight & currentMouseState_)
			{
				int diffX = x - mouseRDragX_; mouseRDragX_ = x;
				int diffY = mouseRDragY_ - y; mouseRDragY_ = y;

				mouseMoveCall(thisState, MouseButtonRight, 
							  currentEntry_->subMouseDragRightList,
						x, y, diffX, diffY);
			}
		}
	}
}

void GameState::mouseMoveCall(const unsigned state, MouseButton button, 
							  StateIList &currentList, 
							  int mx, int my,
							  int dx, int dy)
{
	if (!currentList.empty())
	{
		bool skipRest = false;
		StateIList::iterator subItor;
		for (subItor = currentList.begin();
			subItor != currentList.end();
			subItor++)
		{
			(*subItor)->mouseDrag(state, button, mx, my, dx, dy, skipRest);
			if (checkStimulate()) return;
			if (skipRest) break;
		}
	}
}

void GameState::mouseDown(MouseButton button, int x, int y)
{
	mouseUpDown(button, true, x, y);

	if (doubleClickClock_.getTimeDifference() < 0.25 &&
		abs(mouseDoubleX_ - x) <= 4 &&
		abs(mouseDoubleY_ - y) <= 4)
	{
		switch (button)
		{
		case MouseButtonLeft:
			mouseUpDown(MouseButtonLeftDoubleClick, true, x, y);
			break;
		case MouseButtonMiddle:
			mouseUpDown(MouseButtonMiddleDoubleClick, true, x, y);
			break;
		case MouseButtonRight:
			mouseUpDown(MouseButtonRightDoubleClick, true, x, y);
			break;
		}
	}
	mouseDoubleX_ = x;
	mouseDoubleY_ = y;
}

void GameState::mouseUp(MouseButton button, int x, int y)
{
	mouseUpDown(button, false, x, y);
}

void GameState::mouseUpDown(MouseButton button, bool down, int x, int y)
{
	bool skipRest = false;
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;

		StateIList *currentList = 0;
		if (down)
		{
			if (button <= MouseButtonLeftDoubleClick)
			{
				currentMouseState_ |= (unsigned) button;
			}
			switch(button)
			{
			case MouseButtonRight:
				mouseRDragX_ = x; mouseRDragY_ = y;
				currentList = &currentEntry_->subMouseDownRightList;
				break;
			case MouseButtonMiddle:
				mouseMDragX_ = x; mouseMDragY_ = y;
				currentList = &currentEntry_->subMouseDownMiddleList;
				break;
			default:
				mouseLDragX_ = x; mouseLDragY_ = y;
				currentList = &currentEntry_->subMouseDownLeftList;
				break;
			}
		}
		else
		{
			if (button <= MouseButtonLeftDoubleClick)
			{
				currentMouseState_ ^= (unsigned) button;
			}
			switch(button)
			{
			case MouseButtonRight:
				currentList = &currentEntry_->subMouseUpRightList;
				break;
			case MouseButtonMiddle:
				currentList = &currentEntry_->subMouseUpMiddleList;
				break;
			default:
				currentList = &currentEntry_->subMouseUpLeftList;
				break;
			}
		}

		if (!currentList->empty())
		{
			StateIList::iterator subItor;
			for (subItor = currentList->begin();
				subItor != currentList->end();
				subItor++)
			{
				if (down) 
				{
					(*subItor)->mouseDown(thisState, button, x, y, skipRest);
				}
				else 
				{
					(*subItor)->mouseUp(thisState, button, x, y, skipRest);
				}
				if (checkStimulate()) return;
				if (skipRest) break;
			}
		}
	}
}

void GameState::simulate(float simTime)
{
	if (checkStimulate()) return;
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;

		if (frameCount_ > int(stateTimeLogging_)) clearTimers(true);

		timerClock_.getTicksDifference();
		int timerCount = 0;

		std::list<GameStateSubEntry>::iterator itor;
		for (itor = thisEntry->loopList.begin();
			itor != thisEntry->loopList.end();
			itor++)
		{
			StateIList::iterator subItor;
			for (subItor = itor->subLoopList.begin();
				subItor != itor->subLoopList.end();
				subItor++, timerCount++)
			{
				GameStateI *stateI = (*subItor);
				currentStateI_ = stateI;
				stateI->simulate(thisState, simTime);
				if (checkStimulate()) return;

				timers_[timerCount % 50].simulateTime += 
					timerClock_.getTicksDifference();
				timers_[timerCount % 50].gameStateI = stateI;
			}

			// Draw after all other elements as the camera
			// needs to up to date with respect to all other
			// changes made in the simulate loop
			itor->current->simulate(thisState, simTime);
			if (checkStimulate()) return;
		}

		if (!thisEntry->subKeyList.empty())
		{
			unsigned int historySize;
			unsigned int bufferSize;

			char *buffer  = 
				Keyboard::instance()->getkeyboardbuffer(bufferSize);
			KeyboardHistory::HistoryElement *history = 
				Keyboard::instance()->getkeyboardhistory(historySize);
			unsigned int keyState = 
				Keyboard::instance()->getKeyboardState();

			bool skipRest = false;
			StateIList::iterator subItor;
			for (subItor = thisEntry->subKeyList.begin();
				subItor != thisEntry->subKeyList.end();
				subItor++)
			{
				(*subItor)->keyboardCheck(thisState, simTime, buffer, keyState, 
					history, historySize, skipRest);
				if (checkStimulate()) return;
				if (skipRest) break;
			}
		}

		if (!thisEntry->condStimList.empty())
		{
			StiulusIList::iterator itor;
			for (itor = thisEntry->condStimList.begin();
				itor != thisEntry->condStimList.end();
				itor++)
			{
				SimulusIPair &p = *itor;
				if (p.first->acceptStateChange(thisState, p.second, simTime))
				{
					if (stateLogging_)
					{
						Logger::log(S3D::formatStringBuffer("%s::acceptStateChange(%i, %i)", 
							name_.c_str(), thisState, p.second));
					}
					setState(itor->second);
					return;
				}
				if (checkStimulate()) return;
			}
		}
	}
}

void GameState::draw()
{
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;

		frameCount_ ++;

		timerClock_.getTicksDifference();
		int timerCount = 0;

		std::list<GameStateSubEntry>::iterator itor;
		for (itor = thisEntry->loopList.begin();
			itor != thisEntry->loopList.end();
			itor++)
		{
			itor->current->draw(thisState);

			StateIList::iterator subItor;
			for (subItor = itor->subLoopList.begin();
				subItor != itor->subLoopList.end();
				subItor++, timerCount++)
			{
				GameStateI *stateI = (*subItor);
				currentStateI_ = stateI;
				stateI->draw(thisState);
				if (checkStimulate()) return;

				timers_[timerCount % 50].drawTime += 
					timerClock_.getTicksDifference();
				timers_[timerCount % 50].gameStateI = stateI;
			}
		}
	}
}

void GameState::setState(const unsigned state)
{
	if (stateLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::setState(%i)", name_.c_str(), state));
	}

	clearTimers();

	currentState_ = state;
	currentEntry_ = 0;
	pendingStimulus_ = UINT_MAX;
	std::map<unsigned, GameStateEntry>::iterator itor = stateList_.find(state);
	if (itor != stateList_.end())
	{
		currentEntry_ = &itor->second;

		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;

		if (!thisEntry->enterStateList.empty())
		{
			StateIList::iterator subItor;
			for (subItor = thisEntry->enterStateList.begin();
				subItor != thisEntry->enterStateList.end();
				subItor++)
			{
				GameStateI *s = (*subItor);
				s->enterState(thisState);
			}
		}

		if (checkStimulate()) return;
	}
	else
	{
		S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
			"%s: Failed to find state %i", 
			name_.c_str(),
			state));
	}

	if (stateLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::setStateFinished(%i)", name_.c_str(), state));
	}
}

bool GameState::checkStimulate()
{
	if (pendingStimulus_ != UINT_MAX)
	{
		std::map<unsigned, unsigned>::iterator itor = 
			currentEntry_->stimList.find(pendingStimulus_);
		if (itor != currentEntry_->stimList.end())
		{
			pendingStimulus_ = UINT_MAX;
			setState(itor->second);
			return true;
		}
		else
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"%s: Failed to find stimulus %i in state %i", 
				name_.c_str(),
				pendingStimulus_, currentState_));
		}
	}

	return false;
}

void GameState::stimulate(const unsigned stimulus)
{
	if (stateLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::stimulate(%i)", name_.c_str(), stimulus));
	}

	pendingStimulus_ = stimulus;
}

GameState::GameStateEntry* GameState::getEntry(const unsigned state)
{
	GameStateEntry *foundEntry = 0;
	std::map<unsigned, GameStateEntry>::iterator itor = stateList_.find(state);
	if (itor == stateList_.end())
	{
		GameStateEntry newEntry;
		stateList_[state] = newEntry;

		foundEntry = &stateList_[state];

		if (currentState_ == state)
		{
			currentEntry_ = &stateList_[state];
		}
	}
	else
	{
		foundEntry = &itor->second;
	}

	return foundEntry;
}

GameState::GameStateSubEntry* GameState::getSubEntry(const unsigned state, 
													 GameStateI *entry)
{
	GameStateEntry *foundEntry = getEntry(state);

	GameStateSubEntry *foundSubEntry = 0;
	std::list<GameStateSubEntry>::iterator subItor;
	for (subItor = foundEntry->loopList.begin();
		subItor != foundEntry->loopList.end();
		subItor++)
	{
		if (subItor->current == entry)
		{
			foundSubEntry = &(*subItor);
		}
	}

	if (!foundSubEntry)
	{
		GameStateSubEntry newEntry;
		newEntry.current = entry;

		foundEntry->loopList.push_back(newEntry);
		foundSubEntry = &foundEntry->loopList.back();
	}

	return foundSubEntry;
}

void GameState::addStateLoop(const unsigned state, GameStateI *entry, 
							 GameStateI *subEntry)
{
	getSubEntry(state, entry)->subLoopList.push_back(subEntry);
}

void GameState::addStateEntry(const unsigned state, GameStateI *subEntry)
{
	getEntry(state)->enterStateList.push_back(subEntry);
}

void GameState::addStateKeyEntry(const unsigned state, GameStateI *subEntry)
{
	getEntry(state)->subKeyList.push_back(subEntry);
}

void GameState::addStateMouseDownEntry(const unsigned state, 
									   const unsigned buttons, 
									   GameStateI *subEntry)
{
	if (buttons & MouseButtonLeft)
	{
		getEntry(state)->subMouseDownLeftList.push_back(subEntry);
	}
	if (buttons & MouseButtonRight)
	{
		getEntry(state)->subMouseDownRightList.push_back(subEntry);
	}
	if (buttons & MouseButtonMiddle)
	{
		getEntry(state)->subMouseDownMiddleList.push_back(subEntry);
	}
}

void GameState::addStateMouseUpEntry(const unsigned state, 
									 const unsigned buttons, 
									 GameStateI *subEntry)
{
	if (buttons & MouseButtonLeft)
	{
		getEntry(state)->subMouseUpLeftList.push_back(subEntry);
	}
	if (buttons & MouseButtonRight)
	{
		getEntry(state)->subMouseUpRightList.push_back(subEntry);
	}
	if (buttons & MouseButtonMiddle)
	{
		getEntry(state)->subMouseUpMiddleList.push_back(subEntry);
	}
}

void GameState::addStateMouseDragEntry(const unsigned state, 
									   const unsigned buttons, 
									   GameStateI *subEntry)
{
	if (buttons & MouseButtonLeft)
	{
		getEntry(state)->subMouseDragLeftList.push_back(subEntry);
	}
	if (buttons & MouseButtonRight)
	{
		getEntry(state)->subMouseDragRightList.push_back(subEntry);
	}
	if (buttons & MouseButtonMiddle)
	{
		getEntry(state)->subMouseDragMiddleList.push_back(subEntry);
	}
}

void GameState::addStateMouseWheelEntry(const unsigned state, 
										GameStateI *subEntry)
{
	getEntry(state)->subMouseWheelList.push_back(subEntry);
}

void GameState::addStateStimulus(const unsigned state, 
								 const unsigned stim, 
								 const unsigned nexts)
{
	GameState::GameStateEntry *entry = getEntry(state);
	std::map<unsigned, unsigned>::iterator itor = entry->stimList.find(stim);
	if (itor == entry->stimList.end())
	{
		entry->stimList[stim] = nexts;
	}
	else
	{
		DIALOG_ASSERT(0);
	}
}

void GameState::addStateStimulus(const unsigned state, 
								 GameStateStimulusI *check, 
								 const unsigned nexts)
{
	GameState::GameStateEntry *entry = getEntry(state);
	SimulusIPair pair(check, nexts);
	entry->condStimList.push_back(pair);
}

void GameState::clearTimers(bool printTimers)
{
	unsigned int sinceLastTime = overallTimerClock_.getTicksDifference();
	if (printTimers && stateTimeLogging_ > 0.0f)
	{
		unsigned int simulateTotal = 0, drawTotal = 0;
		for (int i=0; i<50; i++)
		{
			if (timers_[i].gameStateI)
			{
				simulateTotal += timers_[i].simulateTime;
				drawTotal += timers_[i].drawTime;
			}
		}
		int timeLeft = int(sinceLastTime) - int(drawTotal + simulateTotal);
		if (sinceLastTime == 0) sinceLastTime = 1;

		unsigned int drawTotalPer = (100 * drawTotal) / sinceLastTime;
		unsigned int simulateTotalPer = (100 * simulateTotal) / sinceLastTime;
		unsigned int timeLeftPer =  (100 * timeLeft) / sinceLastTime;

		Logger::log(
			"----------------------------------------");
		Logger::log(S3D::formatStringBuffer("%30s Draw : %4u (%3u%%), Simulate : %4u (%3u%%)\nOther : %4i (%3u%%)\n\n", 
			name_.c_str(),
			drawTotal, drawTotalPer,
			simulateTotal, simulateTotalPer,
			timeLeft, timeLeftPer));
		for (int i=0; i<50; i++)
		{
			if (timers_[i].gameStateI)
			{
				unsigned int percentageSimulate =
					(100 * timers_[i].simulateTime) / sinceLastTime;
				unsigned int percentageDraw =
					(100 * timers_[i].drawTime) / sinceLastTime;
				Logger::log(S3D::formatStringBuffer("%2i:%25s - Draw : %4u (%3u%%), Simulate : %4u (%3u%%)", 
					i, 
					timers_[i].gameStateI->getGameStateIName(),
					timers_[i].drawTime, percentageDraw,
					timers_[i].simulateTime, percentageSimulate));

				std::list<GameStatePerfCounter *>::iterator itor;
				for (itor = perfCounters_.begin();
					itor != perfCounters_.end();
					itor++)
				{
					GameStatePerfCounter *counter = *itor;
					const char *state1 = timers_[i].gameStateI->getGameStateIName();
					const char *state2 = counter->getGameStateI()->getGameStateIName();
					if (state1 == state2)
					{
						Logger::log(S3D::formatStringBuffer("%35s +- %4u", 
							counter->getName(),
							counter->getTotal()));
					}
				}
			}
		}
	}

	memset(&timers_, 0, sizeof(timers_));
	frameCount_ = 0;
}

GameStatePerfCounter *GameState::getPerfCounter(const char *name)
{
	std::list<GameStatePerfCounter *>::iterator itor;
	for (itor = perfCounters_.begin();
		itor != perfCounters_.end();
		itor++)
	{
		GameStatePerfCounter *counter = *itor;
		if (0 == strcmp(counter->getName(), name)) return counter;
	}

	GameStatePerfCounter *counter = new GameStatePerfCounter(currentStateI_, name);
	perfCounters_.push_back(counter);
	return counter;
}
