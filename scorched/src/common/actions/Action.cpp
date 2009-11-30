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

#include <actions/Action.h>

const unsigned int Action::ACTION_REFERENCED = -1;
const unsigned int Action::ACTION_NOT_REFERENCED = 0;

Action::Action(unsigned int playerId) : 
	playerId_(playerId),
	renderer_(0), context_(0), 
	actionStartTime_(0)
{
}

Action::~Action()
{
	if (renderer_) delete renderer_;
	renderer_ = 0;
}

void Action::draw()
{
	if (renderer_) renderer_->draw(this);
}

void Action::setActionRender(ActionRenderer *renderer)
{
	if (renderer_) delete renderer_;
	renderer_ = renderer;
}

void Action::setScorchedContext(ScorchedContext *context)
{
	context_ = context;
}

ScorchedContext *Action::getScorchedContext()
{
	return context_;
}

void Action::simulate(fixed frameTime, bool &removeAction)
{
	if (renderer_) renderer_->simulate(this, frameTime.asFloat(), removeAction);
}

ActionRenderer::ActionRenderer()
{
}

ActionRenderer::~ActionRenderer()
{
}

void ActionRenderer::simulate(Action *action, float frametime, bool &removeAction)
{
}

SpriteAction::SpriteAction(ActionRenderer *render) : 
	Action(ACTION_NOT_REFERENCED)
{
	setActionRender(render);
}

SpriteAction::~SpriteAction()
{
}

void SpriteAction::init()
{
}
