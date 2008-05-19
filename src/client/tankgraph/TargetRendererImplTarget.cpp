////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <tankgraph/TargetRendererImplTarget.h>
#include <tankgraph/RenderObjectLists.h>
#include <target/TargetLife.h>
#include <target/TargetState.h>
#include <landscape/Landscape.h>
#include <landscape/ShadowMap.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLState.h>
#include <client/ScorchedClient.h>
#include <engine/ActionController.h>
#include <graph/ModelRendererStore.h>
#include <graph/OptionsDisplay.h>
#include <float.h>

TargetRendererImplTarget::TargetRendererImplTarget(Target *target,
	ModelID model, ModelID burntModel, 
	float scale, float color) :
	modelId_(model), burntModelId_(burntModel),
	target_(target),
	canSeeTank_(false), burnt_(false),
	shieldHit_(0.0f), totalTime_(0.0f),
	targetTips_(target),
	scale_(scale), color_(color),
	fade_(1.0f), distance_(0.0f), size_(2.0f)
{
	modelRenderer_ = new ModelRendererSimulator(
		ModelRendererStore::instance()->loadModel(model));
	burntModelRenderer_ = new ModelRendererSimulator(
		ModelRendererStore::instance()->loadModel(burntModel));
}

TargetRendererImplTarget::~TargetRendererImplTarget()
{
	delete modelRenderer_;
	delete burntModelRenderer_;
}

void TargetRendererImplTarget::addToLists(float distance, RenderObjectLists &renderList)
{
	distance_ = distance;
	canSeeTank_ = false;
	if (!target_->getAlive()) return;

	// Get target size
	size_ = getTargetSize(target_);

	// Don't draw the tank/target if we are drawing shadows and shadows are off
	// for this target
	if (target_->getTargetState().getDisplayHardwareShadow())
	{
		renderList.getShadowList().add(this);
	}

	// Check we can see the target
	if (!GLCameraFrustum::instance()->
		sphereInFrustum(target_->getLife().getFloatPosition(), 
		size_ / 2.0f,
		GLCameraFrustum::FrustrumRed))
	{
		return;
	}
	canSeeTank_ = true;

	// Figure out the drawing distance
	fade_ = getTargetFade(target_, distance, size_ * 2.0f);

	// Are we drawing a tree
	bool tree = false;
	if (burnt_)
	{
		if (burntModelId_.getType()[0] == 'T') tree = true;
	}
	else 
	{
		if (modelId_.getType()[0] == 'T') tree = true;
	}

	// Add the 2d item to draw
	if (target_->getName()[0])
	{
		renderList.get2DList().add(this);
	}

	// Add the 3d item to draw
	if (tree) renderList.getTreeList().add(this);
	else renderList.getModelList().add(this);
}

void TargetRendererImplTarget::simulate(float frameTime)
{
	if (!target_->getAlive()) return;

	totalTime_ += frameTime;
	if (shieldHit_ > 0.0f)
	{
		shieldHit_ -= frameTime / 25.0f;
		if (shieldHit_ < 0.0f) shieldHit_ = 0.0f;
	}

	if (burnt_) burntModelRenderer_->simulate(frameTime * 20.0f);
	else modelRenderer_->simulate(frameTime * 20.0f);
}

void TargetRendererImplTarget::render()
{
	createParticle(target_);
	storeTarget2DPos(target_);

	// Draw texture shadows (if hardware shadows aren't on)
	if (target_->getTargetState().getDisplayShadow() &&
		Landscape::instance()->getShadowMap().shouldAddShadow())
	{
		Landscape::instance()->getShadowMap().addCircle(
			target_->getLife().getFloatPosition()[0], 
			target_->getLife().getFloatPosition()[1], 
			target_->getLife().getSize().Max().asFloat() + 2.0f,
			fade_);
	}

	// Draw the target model
	glColor4f(color_, color_, color_, 1.0f);
	glPushMatrix();
		glTranslatef(
			target_->getLife().getFloatPosition()[0], 
			target_->getLife().getFloatPosition()[1], 
			target_->getLife().getFloatPosition()[2]);
		glMultMatrixf(target_->getLife().getFloatRotMatrix());
		glScalef(scale_, scale_, scale_);
		if (burnt_) burntModelRenderer_->drawBottomAligned(distance_, fade_);
		else modelRenderer_->drawBottomAligned(distance_, fade_);
	glPopMatrix();
}

void TargetRendererImplTarget::render2D()
{
	// Add the tooltip that displays the tank info
	GLWToolTip::instance()->addToolTip(&targetTips_.targetTip,
		float(posX_) - 10.0f, float(posY_) - 10.0f, 20.0f, 20.0f);
}

void TargetRendererImplTarget::renderShadow()
{
	if (!GLCameraFrustum::instance()->
		sphereInFrustum(target_->getLife().getFloatPosition(), 
		size_ / 2.0f,
		GLCameraFrustum::FrustrumRed))
	{
		return;
	}

	glPushMatrix();
		glTranslatef(
			target_->getLife().getFloatPosition()[0], 
			target_->getLife().getFloatPosition()[1], 
			target_->getLife().getFloatPosition()[2]);
		glMultMatrixf(target_->getLife().getFloatRotMatrix());
		glScalef(scale_, scale_, scale_);
		if (burnt_) burntModelRenderer_->drawBottomAligned(FLT_MAX, 1.0f, false);
		else modelRenderer_->drawBottomAligned(FLT_MAX, 1.0f, false);
	glPopMatrix();
}

void TargetRendererImplTarget::drawParticle(float distance)
{
	if (!canSeeTank_) return;

	drawParachute(target_);
	drawShield(target_, shieldHit_, totalTime_);
}

void TargetRendererImplTarget::shieldHit()
{
	shieldHit_ = 0.25f;
}

void TargetRendererImplTarget::fired()
{
}

void TargetRendererImplTarget::targetBurnt()
{
	burnt_ = true;
}
