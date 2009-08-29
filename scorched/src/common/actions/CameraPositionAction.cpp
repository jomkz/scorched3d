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

#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <actions/CameraPositionAction.h>

CameraPositionAction::CameraPositionAction(FixedVector &showPosition,
	fixed showTime,
	unsigned int priority) : 
	Action(-1),
	totalTime_(0), showTime_(showTime),
	showPosition_(showPosition), showPriority_(priority)
{

}

CameraPositionAction::~CameraPositionAction()
{
	if (!context_->getServerMode())
	{
		CameraPositionActionRegistry::rmCameraPositionAction(this);
	}
}

void CameraPositionAction::init()
{
	if (!context_->getServerMode())
	{
		CameraPositionActionRegistry::addCameraPositionAction(this);
	}
}

void CameraPositionAction::simulate(fixed frameTime, bool &remove)
{
	totalTime_ += frameTime;
	if (totalTime_ > showTime_)
	{
		remove = true;
	}

	Action::simulate(frameTime, remove);
}

std::set<CameraPositionAction *> CameraPositionActionRegistry::actions_;
CameraPositionAction *CameraPositionActionRegistry::currentAction_ = 0;

void CameraPositionActionRegistry::addCameraPositionAction(CameraPositionAction *action)
{
	actions_.insert(action);
}

void CameraPositionActionRegistry::rmCameraPositionAction(CameraPositionAction *action)
{
	if (currentAction_ == action) currentAction_ = 0;
	actions_.erase(action);
}

CameraPositionAction *CameraPositionActionRegistry::getCurrentAction()
{
	// Check if the current action is still active
	if (actions_.find(currentAction_) == actions_.end())
	{
		// If the current action is not still going get another one
		currentAction_ = getCurrentBest();
	}
	else 
	{
		// If it is check that it is the best action
		CameraPositionAction *bestAction = getCurrentBest();
		if (bestAction)
		{
			if (bestAction->getShowPriority() > currentAction_->getShowPriority())
			{
				currentAction_ = bestAction;
			}
		}
	}

	return currentAction_;
}

CameraPositionAction *CameraPositionActionRegistry::getCurrentBest()
{
	CameraPositionAction *currentBest = 0;
	if (!actions_.empty())
	{
		std::set<CameraPositionAction *>::iterator itor;
		for (itor = actions_.begin();
			itor != actions_.end();
			itor++)
		{
			CameraPositionAction *action = (*itor);
			
			// Check that this action is near the beginning
			fixed currentTime = action->getScorchedContext()->
				getSimulator().getCurrentTime();
			fixed actionTime = action->getActionStartTime();
			if (currentTime - actionTime < 1)
			{
				// Is there an action to beat
				if (!currentBest) currentBest = action;
				else
				{
					// Yes, so check if it beats it
					if (currentBest->getShowPriority() < action->getShowPriority())
					{
						// There is a new current best
						currentBest = action;
					}
				}
			}
		}
	}
	return currentBest;
}

