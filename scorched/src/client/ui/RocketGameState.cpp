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

#include <ui/RocketGameState.h>
#include <ui/LoadPNG.h>
#include <ui/AnimatedIslandDecoratorInstancer.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>
#include <graph/Main2DCamera.h>

static Rocket::Core::Context* context = 0;
RocketGameState *RocketGameState::instance_ = 0;

RocketGameState *RocketGameState::instance()
{
	if (!instance_)
	{
		instance_ = new RocketGameState;
	}

	return instance_;
}

RocketGameState::RocketGameState() :
	openglRenderer_(0),
	systemInterface_(0),
	fileInterface_(0)
{
}

RocketGameState::~RocketGameState()
{
	delete openglRenderer_;
	delete systemInterface_;
	delete fileInterface_;
}

void RocketGameState::create()
{
	// Rocket initialisation.
	openglRenderer_ = new RocketRenderInterfaceOpenGL();
	Rocket::Core::SetRenderInterface(openglRenderer_);
	systemInterface_ = new RocketSystemInterface();
	Rocket::Core::SetSystemInterface(systemInterface_);
	fileInterface_ = new RocketFileInterface();
	Rocket::Core::SetFileInterface(fileInterface_);

	Rocket::Core::Initialise();
	Rocket::Controls::Initialise();

	std::string veraFile = S3D::getDataFile("data/fonts/dejavusans.ttf");
	Rocket::Core::FontDatabase::LoadFontFace(Rocket::Core::String(veraFile.c_str()));

	// Create the main Rocket context and set it on the shell's input layer.
	int windowWidth = Main2DCamera::instance()->getViewPort().getWidth();
	int windowHeight = Main2DCamera::instance()->getViewPort().getHeight();
	context = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(windowWidth, windowHeight));
	if (!context)
	{
		S3D::dialogExit("Scorched 3D Display", 
				"ERROR: Failed to create rocket context");
	}

	Rocket::Core::DecoratorInstancer* islandDecorator = new AnimatedIslandDecoratorInstancer();
	Rocket::Core::Factory::RegisterDecoratorInstancer("island", islandDecorator);
	islandDecorator->RemoveReference();

	std::string demoFile = S3D::getDataFile("data/rocket/demo.rml");
	Rocket::Core::ElementDocument* document = context->LoadDocument(demoFile.c_str());
	if (document)
	{
		document->Show();
	}

	demoFile = S3D::getDataFile("data/rocket/islandback.rml");
	document = context->LoadDocument(demoFile.c_str());
	if (document)
	{
		document->Show();
	}
}

void RocketGameState::destroy()
{
	context->RemoveReference();
	Rocket::Core::Shutdown();
}

void RocketGameState::draw()
{
	dTimer_.getTimeDifference();
	static bool firstTime = true;
	if (firstTime)
	{
		firstTime = false;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	context->Update();
	context->Render();
	drawTime_ += dTimer_.getTimeDifference();

	swapBuffers();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	clearTime_ += dTimer_.getTimeDifference();
}

void RocketGameState::swapBuffers()
{
 	SDL_GL_SwapBuffers();
}

static unsigned int GetKeyModifierState()
{
	unsigned int fullState = SDL_GetModState();
	unsigned int resultState = 0;
	if (fullState & KMOD_CAPS)
	{
		resultState |= Rocket::Core::Input::KM_CAPSLOCK;
	}
	if (fullState & KMOD_SHIFT)
	{
		resultState |= Rocket::Core::Input::KM_SHIFT;
	}
	if (fullState & KMOD_NUM)
	{
		resultState |= Rocket::Core::Input::KM_NUMLOCK;
	}
	if (fullState & KMOD_CTRL)
	{
		resultState |= Rocket::Core::Input::KM_CTRL;
	}
	if (fullState & KMOD_ALT)
	{
		resultState |= Rocket::Core::Input::KM_ALT;
	}

	return resultState;
}

void RocketGameState::processMouseEvent(SDL_Event &evt)
{
	switch (evt.type) 
	{
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		{
			switch (evt.button.button) 
			{
			case SDL_BUTTON_LEFT:
				if (evt.type == SDL_MOUSEBUTTONDOWN) {
					context->ProcessMouseButtonDown(0, GetKeyModifierState());
				} else {
					context->ProcessMouseButtonUp(0, GetKeyModifierState());
				}
				break;
			case SDL_BUTTON_MIDDLE:
				if (evt.type == SDL_MOUSEBUTTONDOWN) {
					context->ProcessMouseButtonDown(2, GetKeyModifierState());
				} else {
					context->ProcessMouseButtonUp(2, GetKeyModifierState());
				}
				break;
			case SDL_BUTTON_RIGHT:
				if (evt.type == SDL_MOUSEBUTTONDOWN) {
					context->ProcessMouseButtonDown(1, GetKeyModifierState());
				} else {
					context->ProcessMouseButtonUp(1, GetKeyModifierState());
				}
				break;
			case SDL_BUTTON_WHEELUP:
				context->ProcessMouseWheel(120, GetKeyModifierState());
				break;
			case SDL_BUTTON_WHEELDOWN:
				context->ProcessMouseWheel(-120, GetKeyModifierState());
				break;
			}
		}
		break;
	case SDL_MOUSEMOTION:
		context->ProcessMouseMove(evt.button.x, evt.button.y, GetKeyModifierState());
		break;
	}
}