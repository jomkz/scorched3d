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
#include <tankgraph/TargetRendererImplTarget.h>
#include <tankgraph/RenderTracer.h>
#include <tankgraph/RenderGeoms.h>
#include <graph/OptionsDisplay.h>
#include <graph/ModelRendererTree.h>
#include <tank/TankContainer.h>
#include <target/TargetState.h>
#include <client/ScorchedClient.h>
#include <common/OptionsScorched.h>
#include <engine/ActionController.h>
#include <GLEXT/GLGlobalState.h>
#include <landscape/Landscape.h>
#include <land/VisibilityPatchGrid.h>
#include <sky/Sky.h>
#include <algorithm>

RenderTargets *RenderTargets::instance_ = 0;

RenderTargets *RenderTargets::instance()
{
	if (!instance_)
	{
		instance_ = new RenderTargets;
	}
	return instance_;
}

RenderTargets::RenderTargets() :
	treesDrawn_(0), targetsDrawn_(0)
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
	VisibilityPatchInfo &patchInfo = VisibilityPatchGrid::instance()->getPatchInfo();
	stepTime += simTime;

	// step size = 1.0 / physics fps = steps per second
	const float stepSize = 1.0f / float(ScorchedClient::instance()->getOptionsGame().getPhysicsFPS());
	if (stepTime >= stepSize)
	{
		float time = stepTime * ScorchedClient::instance()->getActionController().getFast().asFloat();

		void *currentPatchPtr = 0, *currentObject = 0;
		TargetListIterator patchItor(patchInfo.getTargetVisibility());
		TargetVisibilityIterator itor;
		while (currentPatchPtr = patchItor.getNext())
		{
			TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

			itor.init(currentPatch->getTargets());
			while (currentObject = itor.getNext())
			{
				Target *target = (Target *) currentObject;
				TargetRendererImpl *renderImpl = (TargetRendererImpl *) target->getRenderer();
				renderImpl->simulate(time);
			}
		}

		itor.init(TargetVisibilityPatch::getLargeTargets());
		while (currentObject = itor.getNext())
		{
			Target *target = (Target *) currentObject;
			TargetRendererImpl *renderImpl = (TargetRendererImpl *) target->getRenderer();
			renderImpl->simulate(time);
		}

		stepTime = 0.0f;
	}
}

void RenderTargets::Renderer3D::enterState(const unsigned state)
{

}

static void drawTargetShadows(TargetVisibilityIterator &itor, float distance) 
{
	void *currentObject = 0;
	while (currentObject = itor.getNext())
	{
		Target *target = (Target *) currentObject;
		if (target->getTargetState().getDisplayHardwareShadow())
		{
			if (target->isTarget())
			{
				TargetRendererImplTarget *renderImpl = (TargetRendererImplTarget *) target->getRenderer();
				renderImpl->renderShadow(distance);
			}
			else
			{
				TargetRendererImplTank *renderImpl = (TargetRendererImplTank *) target->getRenderer();
				renderImpl->renderShadow(distance);
			}
		}
	}
}

void RenderTargets::shadowDraw()
{
	VisibilityPatchInfo &patchInfo = VisibilityPatchGrid::instance()->getPatchInfo();

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_DRAW_OBJ");
	{
		unsigned int wantedstate = GLState::BLEND_OFF | 
			GLState::ALPHATEST_OFF | GLState::TEXTURE_OFF;
		GLState glstate(wantedstate);

		{
			GLGlobalState globalState(0);
			ModelRendererTree::setSkipPre(true);
			ModelRendererTree::drawInternalPre(false);

			void *currentPatchPtr = 0;
			TargetListIterator patchItor(patchInfo.getTreeVisibility());
			TargetVisibilityIterator itor;
			while (currentPatchPtr = patchItor.getNext())
			{
				TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

				itor.init(currentPatch->getTrees());
				drawTargetShadows(itor, currentPatch->getDistance());
			}

			ModelRendererTree::setSkipPre(false);
		}
		{
			GLGlobalState globalState(0);
			void *currentPatchPtr = 0;
			TargetListIterator patchItor(patchInfo.getTargetVisibility());
			TargetVisibilityIterator itor;
			while (currentPatchPtr = patchItor.getNext())
			{
				TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

				itor.init(currentPatch->getTargets());
				drawTargetShadows(itor, currentPatch->getDistance());
			}
		}

		{
			GLGlobalState globalState(0);
			TargetVisibilityIterator itor;

			itor.init(TargetVisibilityPatch::getLargeTargets());
			drawTargetShadows(itor, 0.0f);
		}
	}
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_DRAW_OBJ");
}

static void drawTargets(TargetVisibilityIterator &itor, float distance)
{
	void *currentObject = 0;
	while (currentObject = itor.getNext())
	{
		Target *target = (Target *) currentObject;
		if (target->isTarget())
		{
			TargetRendererImplTarget *renderImpl = (TargetRendererImplTarget *) target->getRenderer();
			renderImpl->render(distance);
		}
		else
		{
			TargetRendererImplTank *renderImpl = (TargetRendererImplTank *) target->getRenderer();
			renderImpl->render(distance);
		}
	}
}

void RenderTargets::draw()
{
	// Don't put fully transparent areas into the depth buffer
	unsigned int wantedstate = GLState::BLEND_ON | 
		GLState::ALPHATEST_ON | GLState::TEXTURE_ON | 
		GLState::NORMALIZE_ON | GLState::LIGHTING_ON | 
		GLState::LIGHT1_ON;
	GLState glstate(wantedstate);
	Landscape::instance()->getSky().getSun().setLightPosition(false);

	VisibilityPatchInfo &patchInfo = VisibilityPatchGrid::instance()->getPatchInfo();

	// Trees
	treesDrawn_ = 0;
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "TARGETS_DRAW_TREES");
	if (!OptionsDisplay::instance()->getNoTrees())
	{
		GLGlobalState globalState(0);
		ModelRendererTree::setSkipPre(true);
		ModelRendererTree::drawInternalPre(true);

		void *currentPatchPtr = 0;
		TargetListIterator patchItor(patchInfo.getTreeVisibility());
		TargetVisibilityIterator itor;
		while (currentPatchPtr = patchItor.getNext())
		{
			TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

			itor.init(currentPatch->getTrees());
			drawTargets(itor, currentPatch->getDistance());

			treesDrawn_+=currentPatch->getTrees().size();
		}

		ModelRendererTree::setSkipPre(false);
	}
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "TARGETS_DRAW_TREES");

	// Models
	targetsDrawn_ = 0;
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "TARGETS_DRAW_MODELS");
	{
		GLGlobalState globalState(0);

		void *currentPatchPtr = 0, *currentObject = 0;
		TargetListIterator patchItor(patchInfo.getTargetVisibility());
		TargetVisibilityIterator itor;
		while (currentPatchPtr = patchItor.getNext())
		{
			TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

			itor.init(currentPatch->getTargets());
			drawTargets(itor, currentPatch->getDistance());

			targetsDrawn_+=currentPatch->getTargets().size();
		}

		{
			itor.init(TargetVisibilityPatch::getLargeTargets());
			drawTargets(itor, 0.0f);

			targetsDrawn_+=TargetVisibilityPatch::getLargeTargets().size();
		}
	}
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "TARGETS_DRAW_MODELS");
}

static void drawTargets2D(TargetVisibilityIterator &itor, float distance)
{
	void *currentObject = 0;
	while (currentObject = itor.getNext())
	{
		Target *target = (Target *) currentObject;
		if (target->isTarget())
		{
			TargetRendererImplTarget *renderImpl = (TargetRendererImplTarget *) target->getRenderer();
			renderImpl->render2D(distance);
		}
		else
		{
			TargetRendererImplTank *renderImpl = (TargetRendererImplTank *) target->getRenderer();
			renderImpl->render2D(distance);
		}
	}
}

void RenderTargets::draw2d()
{
	VisibilityPatchInfo &patchInfo = VisibilityPatchGrid::instance()->getPatchInfo();

	// 2D
	{
		void *currentPatchPtr = 0;
		TargetListIterator patchItor(patchInfo.getTargetVisibility());
		TargetVisibilityIterator itor;
		while (currentPatchPtr = patchItor.getNext())
		{
			TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

			itor.init(currentPatch->getTooltips());
			drawTargets2D(itor, currentPatch->getDistance());
		}
	}
}
