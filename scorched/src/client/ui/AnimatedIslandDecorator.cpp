////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <ui/AnimatedIslandDecorator.h>
#include <GLEXT/GLCameraFrustum.h>
#include <graph/MainCamera.h>
#include <graph/Main2DCamera.h>
#include <graph/OptionsDisplay.h>
#include <engine/Simulator.h>
#include <engine/MainLoop.h>
#include <tankgraph/RenderTargets.h>
#include <landscape/Landscape.h>
#include <client/ScorchedClient.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>
#include <movement/TargetMovement.h>
#include <image/ImageFactory.h>

AnimatedIslandDecorator::AnimatedIslandDecorator() :
	rotation_(S3D_HALFPI)
{
}

AnimatedIslandDecorator::~AnimatedIslandDecorator()
{
}

bool AnimatedIslandDecorator::Initialise() 
{
	return true;
}

Rocket::Core::DecoratorDataHandle AnimatedIslandDecorator::GenerateElementData(Rocket::Core::Element* element)
{
	return 0;
}

void AnimatedIslandDecorator::ReleaseElementData(Rocket::Core::DecoratorDataHandle element_data)
{

}

void AnimatedIslandDecorator::RenderElement(Rocket::Core::Element* ROCKET_UNUSED(element), Rocket::Core::DecoratorDataHandle element_data)
{
	static bool initialized = false;
	if (!initialized)
	{
		generate();
		initialized = true;
	}

	drawAnimated();
}

void AnimatedIslandDecorator::generate()
{
	ProgressCounter progressCounter;
	//progressCounter.setUser();
	progressCounter.setNewPercentage(0.0f);

	bool waterMove = OptionsDisplay::instance()->getNoWaterMovement();
	OptionsDisplay::instance()->getNoWaterMovementEntry().setValue(true);

	if (!ScorchedClient::instance()->getLandscapes().readLandscapeDefinitions())
	{
		S3D::dialogExit("Landscape Definitions",
			"Failed to parse landscape definitions");
	}

	LandscapeDefinition definition(
		"data/landscapes/texbackdrop.xml",
		"data/landscapes/defnbackdrop.xml",
		32, 
		"AnimatedBackdropDialog",
		UINT_MAX);

	// Generate new landscape
	ScorchedClient::instance()->getLandscapeMaps().generateMaps(
		ScorchedClient::instance()->getContext(),
		definition,
		&progressCounter);

	// Calculate all the new landscape settings (graphics)
	Landscape::instance()->generate(&progressCounter);

	// Make sure the landscape has been optimized
	Landscape::instance()->recalculateLandscape();
	Landscape::instance()->recalculateRoof();

	OptionsDisplay::instance()->getNoWaterMovementEntry().setValue(waterMove);
}

void AnimatedIslandDecorator::drawAnimated()
{
	float frameTime = clock_.getTimeDifference();
	simulate(frameTime);

	MainCamera::instance()->draw(0);

	GLCameraFrustum::instance()->draw(0);
	Landscape::instance()->calculateVisibility();
	Landscape::instance()->drawShadows();
	Landscape::instance()->drawLand();
	RenderTargets::instance()->render3D.draw(0);
	Landscape::instance()->drawWater();
	Landscape::instance()->drawObjects();

	// Return the viewport to the original
	Main2DCamera::instance()->draw(0);
}

void AnimatedIslandDecorator::simulate(float frameTime)
{
	rotation_ += frameTime * 0.1f;

	MainCamera::instance()->simulate(0, frameTime);
	Landscape::instance()->simulate(frameTime);
	RenderTargets::instance()->render2D.simulate(0, frameTime);
	RenderTargets::instance()->render3D.simulate(0, frameTime);
	ScorchedClient::instance()->getSimulator().simulate();
	ScorchedClient::instance()->getParticleEngine().simulate(0, frameTime);
	ScorchedClient::instance()->getTargetMovement().simulate(
		ScorchedClient::instance()->getContext(), fixed::fromFloat(frameTime));

	MainCamera::instance()->getTarget().setCameraType(TargetCamera::CamFree);
	MainCamera::instance()->getCamera().movePosition(rotation_, 1.3f, 225.0f);
}
