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
#include <ui/RocketEventListenerInstancer.h>
#include <ui/RocketFileInterface.h>
#include <ui/RocketRenderInterfaceOpenGL.h>
#include <ui/RocketSystemInterface.h>
#include <ui/RocketWindowState.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>
#include <GLEXT/GLStateExtension.h>
#include <graph/Main2DCamera.h>
#include <graph/OptionsDisplay.h>
#include <graph/Display.h>
#include <graph/MainCamera.h>
#include <graph/FrameTimer.h>

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

extern char scorched3dAppName[128];
bool RocketGameState::createScorchedWindow()
{
	int width = OptionsDisplay::instance()->getScreenWidth();
	int height = OptionsDisplay::instance()->getScreenHeight();
	bool fullscreen = OptionsDisplay::instance()->getFullScreen();

	SDL_WM_SetCaption(scorched3dAppName, "Scorched3D");
	std::string iconFile = S3D::getDataFile("data/images/tank2.png");

	SDL_Surface *icon = EXT_LoadPNG_RW(SDL_RWFromFile(iconFile.c_str(), "rb"));
	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);

	if (!Display::instance()->changeSettings(width,height,fullscreen)) 
	{
		S3D::dialogMessage("Scorched 3D Display", 
			"ERROR: Failed to set the display mode.\n"
			"Ensure that no other application is exclusively using the graphics hardware.\n"
			"Ensure that the current desktop mode has at least 24 bits colour depth.\n");
		return false;
	}

	int windowWidth = 1024;
	int windowHeight = 768;
	switch (OptionsDisplay::instance()->getDialogSize())
	{
	case 0:
		windowWidth = 1600;
		break;
	case 1:
		windowWidth = 1152;
		break;
	case 3:
		windowWidth = 800;
		break;
	}
	windowHeight = (height * windowWidth) / width;

	//MainCamera::instance()->getCamera().setWindowSize(width, height);
	Main2DCamera::instance()->getViewPort().setWindowSize(windowWidth, windowHeight, 
		width, height);

	glPolygonMode(GL_FRONT, GL_FILL);

	glDisable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	GLfloat fogColor[4]= {0.8f, 0.8f, 0.8f, 1.0f};

	glFogi(GL_FOG_MODE, GL_EXP2);	
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.001f);	
	//glHint(GL_FOG_HINT, GL_DONT_CARE);
	//glFogf(GL_FOG_START, 1.0f);	
	//glFogf(GL_FOG_END, 2.0f);
	//glFogf(GL_FOG_DISTANCE_MODE_NV, GL_EYE_RADIAL_NV);

	glAlphaFunc(GL_GREATER, 0.00f);

	//Console::instance(); // Make sure console is inited
	//GLState::setBaseState(GLState::TEXTURE_ON | GLState::BLEND_OFF | GLState::DEPTH_ON);
	GLStateExtension::setup();

	if (OptionsDisplay::instance()->getOpenGLWarnings() &&
		GLStateExtension::isSoftwareOpenGL())
	{
		S3D::dialogMessage("Scorched 3D Display", 
			S3D::formatStringBuffer(
			"Warning: This computer is not using the graphics card to renderer OpenGL.\n"
			"This may cause the game to play very slowly!\n\n"
			"Please update your graphics drivers from the appropriate website.\n\n"
			"OpenGL Vendor : %s\nOpenGL Renderer : %s",
			glGetString(GL_VENDOR), glGetString(GL_RENDERER)));
	}

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1024, 768, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return true;
}

void RocketGameState::create()
{
	createScorchedWindow();

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

	RocketEventListenerInstancer* event_instancer = new RocketEventListenerInstancer();
	Rocket::Core::Factory::RegisterEventListenerInstancer(event_instancer);
	event_instancer->RemoveReference();

	windowState_ = new RocketWindowState(context);
	windowState_->initialize();
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
	FrameTimer::instance()->draw();
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
				context->ProcessMouseWheel(-1, GetKeyModifierState());
				break;
			case SDL_BUTTON_WHEELDOWN:
				context->ProcessMouseWheel(1, GetKeyModifierState());
				break;
			}
		}
		break;
	case SDL_MOUSEMOTION:
		context->ProcessMouseMove(evt.button.x, evt.button.y, GetKeyModifierState());
		break;
	}
}

void RocketGameState::setState(const std::string &state)
{
	windowState_->setState(state);
}
