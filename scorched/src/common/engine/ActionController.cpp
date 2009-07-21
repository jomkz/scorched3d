////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <engine/ActionController.h>
#include <engine/ScorchedContext.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <list>

ActionController::ActionController() : 
	speed_(1), referenceCount_(0), time_(0), 
	context_(0), lastTraceTime_(0),
	actionTracing_(false), stepTime_(0),
	actionEvents_(false), actionProfiling_(false),
	stopImmediately_(false),
	actionNumber_(0)
{

}

ActionController::~ActionController()
{

}

void ActionController::clear(bool warn)
{
	// New actions
	std::list<Action *>::iterator newItor;
	for (newItor = newActions_.begin();
		newItor != newActions_.end();
		newItor++)
	{
		Action *act = *newItor;
		if (warn)
		{
			Logger::log(S3D::formatStringBuffer("Warning: removing added timed out action %s, %u",
				act->getActionType().c_str(), act->getPlayerId()));
		}
		delete act;
	}
	newActions_.clear();

	// New Last actions
	for (newItor = newLastActions_.begin();
		newItor != newLastActions_.end();
		newItor++)
	{
		Action *act = *newItor;
		if (warn)
		{
			Logger::log(S3D::formatStringBuffer("Warning: removing added timed out action %s, %u",
				act->getActionType().c_str(), act->getPlayerId()));
		}
		delete act;
	}
	newLastActions_.clear();

	// Current actions
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		if (warn)
		{
			Logger::log(S3D::formatStringBuffer("Warning: removing added timed out action %s, %u",
				act->getActionType().c_str(), act->getPlayerId()));
		}
		delete act;
	}
	actions_.clear();

	// Ref count
	referenceCount_ = 0;
}

bool ActionController::allEvents()
{
	// Criteria to add more events :-
	// Check that there are referenced actions in the simulation,
	// and that these referenced actions are not events
	std::list<Action *>::iterator newItor;
	for (newItor = newActions_.begin();
		newItor != newActions_.end();
		newItor++)
	{
		Action *act = *newItor;
		if (act->getPlayerId() != 0) return false;
	}
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		if (act->getPlayerId() != 0) return false;
	}
	return true;
}

void ActionController::logActions()
{
	Logger::log(S3D::formatStringBuffer("ActionLog : Time %.2f, New %i, New Last %i, Ref %i",
		time_.asFloat(),
		(int) newActions_.size(), 
		(int) newLastActions_.size(),
		referenceCount_));
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		Logger::log(S3D::formatStringBuffer("Action : %s", act->getActionType().c_str()));
	}
}

void ActionController::logProfiledActions()
{
	if (!actionProfiling_) return;

	Logger::log("Logging Profiled Actions --------------------");
	int totalCount = 0;
	std::map<std::string, int>::iterator itor;
	for (itor =  actionProfile_.begin();
		itor != actionProfile_.end();
		itor++)
	{
		const std::string &name = (*itor).first;
		int count = (*itor).second;
		totalCount += count;
		Logger::log(S3D::formatStringBuffer("%s - %i", name.c_str(), count));
	}
	Logger::log(S3D::formatStringBuffer("Total - %i", totalCount));
	Logger::log("---------------------------------------------");

	actionProfile_.clear();
}

bool ActionController::noReferencedActions()
{
	bool finished = 
		newActions_.empty() && 
		newLastActions_.empty() &&
		(referenceCount_ == 0);

	if (actionTracing_)
	{
		if (time_ - lastTraceTime_ > 5)
		{
			lastTraceTime_ = time_;
			logActions();
		}
	}

	return finished;
}

void ActionController::resetTime()
{
	time_ = 0;
	lastTraceTime_ = 0;
	stepTime_ = 0;

	syncCheck_.clear();
}

void ActionController::setScorchedContext(ScorchedContext *context)
{
	context_ = context;
}

void ActionController::setFast(fixed speedMult)
{
	speed_ = speedMult;
}

void ActionController::addSyncCheck(const std::string &msg)
{
	if (!stopImmediately_) return;

	DIALOG_ASSERT(context_->getOptionsGame().getActionSyncCheck());
	syncCheck_.push_back(msg);
}

void ActionController::addAction(Action *action)
{
	action->setScorchedContext(context_);
	action->setActionStartTime(time_);

	if (action->getPlayerId() != 0)
	{
		if (context_->getOptionsGame().getActionSyncCheck())
		{
			std::string actionType = action->getActionType();
			std::string actionDetails = action->getActionDetails();
			addSyncCheck(
				S3D::formatStringBuffer("Add Action : %i %s:%s", 
					time_.getInternal(), 
					actionType.c_str(), 
					actionDetails.c_str()));
		}
	}

	newActions_.push_back(action);

	if (actionProfiling_)
	{
		std::map<std::string, int>::iterator findItor =
			actionProfile_.find(action->getActionType());
		if (findItor == actionProfile_.end())
		{
			actionProfile_[action->getActionType()] = 1;
		}
		else
		{
			(*findItor).second++;
		}	
	}
}

void ActionController::addLastAction(Action *action)
{
	newLastActions_.push_back(action);
}

void ActionController::addNewLastActions()
{
	if (newActions_.empty() &&
		referenceCount_ == 0)
	{
		while (!newLastActions_.empty())
		{
			Action *action = newLastActions_.front(); 
			addAction(action);			
			newLastActions_.pop_front();
		}
	}
}

void ActionController::addNewActions()
{
	while (!newActions_.empty())
	{
		Action *action = newActions_.front(); 

		action->setScorchedContext(context_);
		action->setActionStartTime(time_);
		if (action->getPlayerId() != 0) referenceCount_ ++;

		action->init();
		actions_.push_back(action);

		newActions_.pop_front();
	}
}

void ActionController::draw()
{
	// Itterate and draw all of the actions
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		act->draw();
	}
}

void ActionController::simulate(fixed frameTime)
{
	// Ensure any new last actions are added
	addNewLastActions();

	// Ensure any new actions are added
	addNewActions();

	// Add any new events (if allowed)
	/*if (time_ < 10 && !allEvents())
	{
		actionEvents_ = true;
		events_.simulate(frameTime, *context_);
		actionEvents_ = false;
	}*/

	// Itterate and draw all of the actions
	int keepcount=0;
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		bool remove = false;
		act->simulate(frameTime, remove);

		// Ensure that no referenced actions over do their time
		if (act->getPlayerId() != 0)
		{
			if ((time_ - act->getActionStartTime() > 30))
			{
				Logger::log(S3D::formatStringBuffer("Warning: removing timed out action %s",
					act->getActionType().c_str()));
				remove = true;
			}
		}

		// If this action has finished add to list to be removed
		if (remove)
		{
			if (act->getPlayerId() != 0)
			{
				referenceCount_--;
				if (referenceCount_<0) referenceCount_ = 0;

				if (context_->getOptionsGame().getActionSyncCheck())
				{
					std::string actionType = act->getActionType();
					std::string actionDetails = act->getActionDetails();
					addSyncCheck(
						S3D::formatStringBuffer("Rm Action : %i %s:%s", 
							time_.getInternal(), 
							actionType.c_str(), 
							actionDetails.c_str()));
				}
			}

			delete act;
		}
		else
		{
			actions_.actions[keepcount] = act;
			keepcount++;
		}
	}
	actions_.actionCount = keepcount;
}

