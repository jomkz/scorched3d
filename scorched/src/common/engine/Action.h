////////////////////////////////////////////////////////////////////////////////
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

#if !defined(AFX_ACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
#define AFX_ACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_

#include <common/fixed.h>
#include <string>

class Action;
class ScorchedContext;
class ActionRenderer
{
public:
	friend class Action;

	ActionRenderer();
	virtual ~ActionRenderer();

	virtual void draw(Action *action) = 0;
	virtual void simulate(Action *action, float frametime, bool &removeAction);

};

class Action
{
public:
	Action(const char *name, ActionRenderer *render = 0);
	virtual ~Action();

	virtual void init() = 0;

	virtual void draw();
	virtual void simulate(fixed frameTime, bool &removeAction);

	virtual void setActionRender(ActionRenderer *renderer);
	virtual void setScorchedContext(ScorchedContext *context);
	virtual ScorchedContext *getScorchedContext();
	virtual bool getReferenced() { return false; }
	virtual std::string getActionType() { return name_; }
	virtual std::string getActionDetails() { return ""; }

	void setActionStartTime(fixed time) { actionStartTime_ = time; }
	fixed getActionStartTime() { return actionStartTime_; }
	bool getActionEvent() { return actionEvent_; }
	void setActionEvent(bool ae) { actionEvent_ = ae; }
	unsigned int getActionNumber() { return actionNumber_; }
	void setActionNumber(unsigned int number) { actionNumber_ = number; }

protected:
	const char *name_;
	ActionRenderer *renderer_;
	ScorchedContext *context_;
	fixed actionStartTime_;
	unsigned int actionNumber_;
	bool actionEvent_;

};

class SpriteAction : public Action
{
public:
	SpriteAction(ActionRenderer *render = 0);
	virtual ~SpriteAction();

	virtual void init();

};

#endif // !defined(AFX_ACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
