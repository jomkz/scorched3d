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

#include <uiactions/UIParticleAction.h>
#include <scorched3dc/ScorchedUI.h>
#include <client/ScorchedClient.h>
#include <scorched3dc/OgreSystem.h>

UIParticleAction::UIParticleAction(FixedVector &position, XMLEntryParticleIDList &particles) :
	position_(position),
	particles_(particles),
	destroy_(false)
{
	ENSURE_CLIENT_THREAD;
}

UIParticleAction::~UIParticleAction()
{
	ENSURE_UI_THREAD;
}

void UIParticleAction::performUIAction()
{
	if (!destroy_)
	{
		particleInstances_.setListener(this);
		particleInstances_.create(particles_);
		particleInstances_.getSceneNode()->setPosition(
			position_[0].getInternalData() * OgreSystem::OGRE_WORLD_SCALE_FIXED,
			position_[2].getInternalData() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE_FIXED + 10.0f,
			position_[1].getInternalData() * OgreSystem::OGRE_WORLD_SCALE_FIXED);
	}
	else
	{
		delete this;
	}
}

void UIParticleAction::finished(ParticleInstance *instance)
{
	DIALOG_ASSERT(!destroy_);
	destroy_ = true;
	ScorchedClient::getClientUISyncExternal().addActionFromUI(this);
}

