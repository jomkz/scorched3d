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

#include <stdio.h>
#include <graph/FrameTimer.h>
#include <client/ClientChannelManager.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLInfo.h>
#include <GLEXT/GLTexture.h>

FrameTimer *FrameTimer::instance_ = 0;

FrameTimer *FrameTimer::instance()
{
	if (!instance_)
	{
		instance_ = new FrameTimer;
	}

	return instance_;
}

FrameTimer::FrameTimer() : 
	GameStateI("FrameTimer"),
	totalTime_(0.0f), frameCount_(0), 
	lastStateCount_(0), lastTris_(0), lastTextureSets_(0),
	fps_(0.0f)
{

}

FrameTimer::~FrameTimer()
{

}

void FrameTimer::draw(const unsigned state)
{
	frameCount_++;

	lastStateCount_ = GLState::getStateSwitches();
	GLState::resetStateSwitches();

	lastTris_ = GLInfo::getNoTriangles();
	GLInfo::resetNoTriangles();
	
	lastTextureSets_ = GLTexture::getTextureSets();
	GLTexture::resetTextureSets();
}

void FrameTimer::simulate(const unsigned state, float frameTime)
{
	totalTime_ += frameTime;
	if (totalTime_ > 5.0f)
	{
		float timeTaken = frameClock_.getTimeDifference();
		fps_ = float(frameCount_) / timeTaken;
		totalTime_ = 0.0f;
		frameCount_ = 0;

		if (OptionsDisplay::instance()->getFrameTimer())
		{
			ChannelText chText("info", S3D::formatStringBuffer("%.2f frames per second.", fps_));
			chText.setFlags(ChannelText::eNoLog | ChannelText::eNoSound);
			ClientChannelManager::instance()->showText(chText);
		}
	}
}
