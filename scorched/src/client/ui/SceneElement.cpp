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

#include <ui/SceneElement.h>
#include <graph/MainCamera.h>
#include <graph/Main2DCamera.h>
#include <graph/ShotCountDown.h>
#include <GLEXT/GLCameraFrustum.h>
#include <landscape/Landscape.h>
#include <tankgraph/RenderTargets.h>
#include <client/ScorchedClient.h>
#include <client/ClientSimulator.h>

SceneElement::SceneElement(const Rocket::Core::String& tag) :
	Rocket::Core::Element(tag),
	mouseDown_(-1),
	targetCam_("main")
{

}

SceneElement::~SceneElement()
{
}

void SceneElement::ProcessEvent(Rocket::Core::Event& evt)
{
	if (!processEventInternal(evt))
	{
		Rocket::Core::Element::ProcessEvent(evt);
	}
}

bool SceneElement::processEventInternal(Rocket::Core::Event& evt)
{
	if (evt.GetCurrentElement() != this) return false;

	bool mousedown = false;
	if ((mousedown = evt.GetType() == "mousedown") ||
		evt.GetType() == "mouseup")
	{
		float height = GetClientHeight();
		int button = evt.GetParameter<int>("button", -1);
		int mouse_x = evt.GetParameter<int>("mouse_x", -1);
		int mouse_y = int(height) - evt.GetParameter<int>("mouse_y", -1);

		if (mousedown)
		{
			dx_ = mouse_x; dy_ = mouse_y;
			mouseDown_ = button;
			targetCam_.mouseDown((GameState::MouseButton) button, mouse_x, mouse_y);
		}
		else
		{
			mouseDown_ = -1;
			targetCam_.mouseUp((GameState::MouseButton) button, mouse_x, mouse_y);
		}
		return true;
	}
	else if (evt.GetType() == "mousemove")
	{
		if (mouseDown_ != -1)
		{
			float height = GetClientHeight();
			int mouse_x = evt.GetParameter<int>("mouse_x", -1);
			int mouse_y = int(height) - evt.GetParameter<int>("mouse_y", -1);

			targetCam_.mouseDrag((GameState::MouseButton) mouseDown_, mouse_x, mouse_y, 
				mouse_x - dx_, dy_ - mouse_y);

			dx_ = mouse_x; dy_ = mouse_y;
			return true;
		}
	}
	else if (evt.GetType() == "mousescroll")
	{
		int wheel_delta = evt.GetParameter<int>("wheel_delta", -1);
		targetCam_.mouseWheel(wheel_delta * 120);
		return true;
	} 

	return false;
}

void SceneElement::OnRender()
{
	targetCam_.getCamera().setWindowSize((GLsizei) GetClientWidth(), (GLsizei) GetClientHeight());
	targetCam_.getCamera().setWindowOffset((GLsizei) GetAbsoluteLeft(), (GLsizei) GetAbsoluteTop());

	targetCam_.draw();
	GLCameraFrustum::instance()->draw();
	Landscape::instance()->calculateVisibility();
	Landscape::instance()->drawShadows();
	Landscape::instance()->drawLand();
	RenderTargets::instance()->draw3d();
	Landscape::instance()->drawWater();
	Landscape::instance()->drawObjects();
	ScorchedClient::instance()->getClientSimulator().getActionController().draw();
	ScorchedClient::instance()->getParticleEngine().draw();
	targetCam_.drawPrecipitation();

	Main2DCamera::instance()->draw();
	RenderTargets::instance()->draw2d();
	ShotCountDown::instance()->draw();
}
