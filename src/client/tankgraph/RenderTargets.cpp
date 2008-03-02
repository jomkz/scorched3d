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

#include <tankgraph/RenderTargets.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <tankgraph/RenderTracer.h>
#include <tankgraph/RenderGeoms.h>
#include <graph/ModelRendererTree.h>
#include <graph/ModelRendererMesh.h>
#include <graph/OptionsDisplay.h>
#include <tank/TankContainer.h>
#include <target/TargetLife.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <GLEXT/GLCamera.h>
#include <GLEXT/GLGlobalState.h>
#include <landscape/Landscape.h>
#include <landscape/Sky.h>
#include <algorithm>

static inline float approx_distance(float  dx, float dy, float dz)
{
   float approx = (dx * dx) + (dy * dy) + (dz * dz);
   return approx;
}

RenderTargets *RenderTargets::instance_ = 0;

RenderTargets *RenderTargets::instance()
{
	if (!instance_)
	{
		instance_ = new RenderTargets;
	}
	return instance_;
}

RenderTargets::RenderTargets() : createLists_(false)
{
}

RenderTargets::~RenderTargets()
{
}

void RenderTargets::Renderer2D::draw(const unsigned state)
{
	RenderTargets::instance()->draw2d();
}

void RenderTargets::Renderer2D::simulate(const unsigned state, float simTime)
{
	// Simulate the HUD
	TargetRendererImplTankHUD::simulate(simTime);
	TargetRendererImplTankAIM::simulate(simTime);
}

void RenderTargets::Renderer3D::draw(const unsigned state)
{
	RenderTracer::instance()->draw(state);
	RenderGeoms::instance()->draw(state);
	RenderTargets::instance()->draw();
}

void RenderTargets::Renderer3D::simulate(const unsigned state, float simTime)
{
	// Simulate all of the tanks
	std::map<unsigned int, Target *> &targets = 
		ScorchedClient::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets.begin();
		itor != targets.end();
		itor++)
	{
		Target *target = (*itor).second;
		TargetRenderer *renderer = target->getRenderer();
		if (renderer)
		{
			TargetRendererImpl *rendererImpl = (TargetRendererImpl *) renderer;
			rendererImpl->simulate(simTime);
		}
	}

	RenderTargets::instance()->createLists_ = true;
}

void RenderTargets::Renderer3D::enterState(const unsigned state)
{
	RenderTargets::instance()->createLists_ = true;
}

void RenderTargets::createLists()
{
	if (!createLists_) return;
	createLists_ = false;

	// Camera position for LOD
	Vector &campos = GLCamera::getCurrentCamera()->getCurrentPos();

	// Reset everything that is to be drawn
	RenderObjectLists &objectsList = 
		RenderTargets::instance()->renderObjectLists_;
	objectsList.reset();

	std::map<unsigned int, Target *> &targets = 
		ScorchedClient::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets.begin();
		itor != targets.end();
		itor++)
	{
		Target *target = (*itor).second;
		TargetRenderer *renderer = target->getRenderer();
		if (renderer)
		{
			float distance = approx_distance(
				target->getLife().getFloatPosition()[0] - campos[0],
				target->getLife().getFloatPosition()[1] - campos[1],
				target->getLife().getFloatPosition()[2] - campos[2]);

			TargetRendererImpl *rendererImpl = (TargetRendererImpl *) renderer;
			rendererImpl->addToLists(distance, objectsList);
		}
	}
}

void RenderTargets::shadowDraw()
{
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_CREATE_LISTS");
	createLists();
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_CREATE_LISTS");

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_DRAW_OBJ");
	// Shadows
	if (!OptionsDisplay::instance()->getNoGLObjectShadows())
	{
		unsigned int wantedstate = GLState::BLEND_OFF | 
			GLState::ALPHATEST_OFF | GLState::TEXTURE_OFF;

		GLGlobalState globalState(wantedstate);
		RenderObjectList &renderList = renderObjectLists_.getShadowList();
		RenderObject **object = renderList.getObjects();
		for (unsigned int c=0; c<renderList.getCount(); c++, object++)
		{
			(*object)->renderShadow();
		}
	}
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_DRAW_OBJ");
}

void RenderTargets::draw()
{
	createLists();

	// Don't put fully transparent areas into the depth buffer
	unsigned int wantedstate = GLState::BLEND_ON | 
		GLState::ALPHATEST_ON | GLState::TEXTURE_ON | 
		GLState::NORMALIZE_ON | GLState::LIGHTING_ON | 
		GLState::LIGHT1_ON;
	GLState glstate(wantedstate);
	Landscape::instance()->getSky().getSun().setLightPosition(false);

	// Trees
	{
		GLGlobalState globalState(0);
		RenderObjectList &renderList = renderObjectLists_.getTreeList();
		RenderObject **object = renderList.getObjects();
		for (unsigned int c=0; c<renderList.getCount(); c++, object++)
		{
			(*object)->render();
		}
	}

	// Models
	{
		GLGlobalState globalState(0);
		RenderObjectList &renderList = renderObjectLists_.getModelList();
		RenderObject **object = renderList.getObjects();
		for (unsigned int c=0; c<renderList.getCount(); c++, object++)
		{
			(*object)->render();
		}
	}
}

void RenderTargets::draw2d()
{
	createLists();

	// 2D
	{
		RenderObjectList &renderList = renderObjectLists_.get2DList();
		RenderObject **object = renderList.getObjects();
		for (unsigned int c=0; c<renderList.getCount(); c++, object++)
		{
			(*object)->render2D();
		}
	}
}
