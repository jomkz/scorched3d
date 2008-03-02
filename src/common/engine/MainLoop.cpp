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

#include <engine/MainLoop.h>
#include <common/Defines.h>
#ifndef S3D_SERVER
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLInfo.h>
#include <SDL/SDL.h>
#endif

MainLoop::MainLoop() :  exitLoop_(false), lastDrawTime_(0.0f)
{

}

MainLoop::~MainLoop()
{

}

void MainLoop::clear()
{
	newMainLoops_.clear();
	mainLoops_.clear();
	exitLoop_ = false;
}

void MainLoop::addMainLoop(MainLoopI *MainLoop)
{
	newMainLoops_.push_back(MainLoop);
}

void MainLoop::addNew()
{
	while (!newMainLoops_.empty())
	{
		mainLoops_.insert(newMainLoops_.front());
		newMainLoops_.pop_front();
	}
}

void MainLoop::swapBuffers()
{
#ifndef S3D_SERVER
 	SDL_GL_SwapBuffers();
#endif
}

bool MainLoop::mainLoop()
{
	float frameTime = fTimer_.getTimeDifference();
	while (frameTime > 0.0f)
	{
		addNew();

		float eventTime = frameTime;
		simulate(eventTime);
		frameTime -= eventTime;
	}

	return !exitLoop_;
}

void MainLoop::simulate(float frameTime)
{
	std::set<MainLoopI *>::iterator itor;
	for (itor = mainLoops_.begin();
		itor != mainLoops_.end();
		itor++)
	{
		MainLoopI *current = (*itor);
		current->simulate(frameTime);
	}

}

void MainLoop::draw()
{
#ifndef S3D_SERVER
	GLInfo::resetNoTriangles();
	lastDrawTime_ = dTimer_.getTimeDifference();
	static bool firstTime = true;
	if (firstTime)
	{
		firstTime = false;
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	std::set<MainLoopI *>::iterator itor;
	for (itor = mainLoops_.begin();
		itor != mainLoops_.end();
		itor++)
	{
		MainLoopI *current = (*itor);
		current->draw();
	}

	glFlush();
	swapBuffers();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0f);
		glDepthRange(0.0, 1.0);
		glDepthFunc(GL_LESS);
	}
#endif
}
