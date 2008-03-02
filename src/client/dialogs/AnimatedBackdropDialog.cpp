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

#include <dialogs/AnimatedBackdropDialog.h>
#include <dialogs/BackdropDialog.h>
#include <dialogs/ProgressDialog.h>
#include <GLEXT/GLCameraFrustum.h>
#include <graph/MainCamera.h>
#include <graph/Main2DCamera.h>
#include <graph/OptionsDisplay.h>
#include <engine/ActionController.h>
#include <engine/MainLoop.h>
#include <tankgraph/RenderTargets.h>
#include <landscape/Landscape.h>
#include <client/ScorchedClient.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>

AnimatedBackdropDialog *AnimatedBackdropDialog::instance_ = 0;

AnimatedBackdropDialog *AnimatedBackdropDialog::instance()
{
	if (!instance_)
	{
		instance_ = new AnimatedBackdropDialog;
	}
	return instance_;
}

AnimatedBackdropDialog::AnimatedBackdropDialog() : 
	GLWWindow("", 0.0f, 0.0f, 0.0f, 0.0f, 0,
		"The backdrop dialog"),
	rotation_(HALFPI),
	init_(false)
{
	windowLevel_ = 5000000;
}

AnimatedBackdropDialog::~AnimatedBackdropDialog()
{
}

void AnimatedBackdropDialog::init()
{
	init_ = true;

	ProgressCounter progressCounter;
	ProgressDialog::instance()->changeTip();
	progressCounter.setUser(ProgressDialogSync::instance());
	progressCounter.setNewPercentage(0.0f);

	bool waterMove = OptionsDisplay::instance()->getNoWaterMovement();
	OptionsDisplay::instance()->getNoWaterMovementEntry().setValue(true);

	if (!ScorchedClient::instance()->getLandscapes().readLandscapeDefinitions())
	{
		return;
	}

	LandscapeDefinition definition = ScorchedClient::instance()->getLandscapes().getLandscapeDefn(
		"oldstyle");

	// Set the progress dialog nicities
	LandscapeDefinitionsEntry *landscapeDefinition =
		ScorchedClient::instance()->getLandscapes().getLandscapeByName(
			definition.getName());

	// Generate new landscape
	std::list<FixedVector> tankPositions;
	ScorchedClient::instance()->getLandscapeMaps().generateMaps(
		ScorchedClient::instance()->getContext(),
		definition,
		tankPositions,
		&progressCounter);

	// Calculate all the new landscape settings (graphics)
	Landscape::instance()->generate(&progressCounter);

	// Make sure the landscape has been optimized
	Landscape::instance()->reset(&progressCounter);

	OptionsDisplay::instance()->getNoWaterMovementEntry().setValue(waterMove);

	// Hack to get camera synched
	simulate(5.0f);
	ScorchedClient::instance()->getMainLoop().getTimer().getTimeDifference();
}

void AnimatedBackdropDialog::draw()
{
	drawBackground();
	BackdropDialog::instance()->drawLogo();
	BackdropDialog::instance()->drawFooter();
}

void AnimatedBackdropDialog::drawBackground()
{
	if (!init_) init();

	MainCamera::instance()->draw(0);

	GLCameraFrustum::instance()->draw(0);
	Landscape::instance()->drawShadows();
	Landscape::instance()->drawLand();
	RenderTargets::instance()->render3D.draw(0);
	Landscape::instance()->drawWater();
	Landscape::instance()->drawObjects();

	// Return the viewport to the original
	Main2DCamera::instance()->draw(0);
}

void AnimatedBackdropDialog::simulate(float frameTime)
{
	if (!init_) init();

	rotation_ += frameTime * 0.1f;

	MainCamera::instance()->simulate(0, frameTime);
	Landscape::instance()->simulate(frameTime);
	RenderTargets::instance()->render2D.simulate(0, frameTime);
	RenderTargets::instance()->render3D.simulate(0, frameTime);
	ScorchedClient::instance()->getActionController().simulate(0, frameTime);
	ScorchedClient::instance()->getParticleEngine().simulate(0, frameTime);

	MainCamera::instance()->getTarget().setCameraType(TargetCamera::CamFree);
	MainCamera::instance()->getCamera().movePosition(rotation_, 1.3f, 225.0f);
}
