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

#include <uiactions/UITankRenderer.h>
#include <uiactions/UITankModel.h>
#include <uiactions/UITankActiveModel.h>
#include <scorched3dc/ScorchedUI.h>
#include <client/ScorchedClient.h>

UITankRenderer::UITankRenderer(Tank *tank) :
	UITargetRenderer(tank),
	activeModel_(0)
{
	tankWeapon_.setTankRenderer(this);
	shotHistory_.setTankRenderer(this);
}

UITankRenderer::~UITankRenderer()
{
	ENSURE_CLIENT_THREAD
	delete activeModel_;
	activeModel_ = 0;
}

UITankShotHistory &UITankRenderer::getShotHistory() 
{ 
	ENSURE_UI_THREAD
	return shotHistory_; 
}

UITankWeapon &UITankRenderer::getTankWeapon() 
{ 
	ENSURE_UI_THREAD
	return tankWeapon_; 
}

UITankActiveModel *UITankRenderer::getActiveModel() 
{ 
	ENSURE_UI_THREAD
	return activeModel_; 
}

void UITankRenderer::updateRotation()
{
	ENSURE_UI_THREAD
	((UITankModel *) targetModel_)->setRotations();
}

void UITankRenderer::setActive()
{
	ENSURE_UI_THREAD
	activeModel_ = new UITankActiveModel(this);
}

void UITankRenderer::setInactive()
{
	ENSURE_UI_THREAD
	delete activeModel_;
	activeModel_ = 0;
}

UITargetModel *UITankRenderer::createModel()
{
	return new UITankModel(this);
}
