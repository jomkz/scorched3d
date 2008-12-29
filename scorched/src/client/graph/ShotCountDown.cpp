////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <graph/ShotCountDown.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <common/OptionsScorched.h>
#include <common/DefinesString.h>
#include <GLEXT/GLViewPort.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWidget.h>

ShotCountDown *ShotCountDown::instance_ = 0;

ShotCountDown *ShotCountDown::instance()
{
	if (!instance_)
	{
		instance_ = new ShotCountDown;
	}
	return instance_;
}

ShotCountDown::ShotCountDown() : 
	GameStateI("ShotCountDown"),
	counter_(0.0f), blinkTimer_(0.0f), 
	showTime_(true), timerOff_(true)
{
}

ShotCountDown::~ShotCountDown()
{
}

void ShotCountDown::reset(float time)
{
	timerOff_ = (time == 0.0f);
	counter_ = time;
	blinkTimer_ = 0.0f;
	showTime_ = true;
}

void ShotCountDown::simulate(const unsigned state, float simTime)
{
	counter_ -= simTime;
	blinkTimer_ += simTime;

	if (blinkTimer_ > 0.25f)
	{
		if (counter_ < 5.0f)
		{
			showTime_ = !showTime_;
		}
		blinkTimer_ = 0.0f;
	}
}

void ShotCountDown::draw(const unsigned currentstate)
{
	if (timerOff_) return;

	// The remaining time for this shot
	int timeLeft = (int) counter_;

	// Split into seconds and minutes
	div_t split = div(timeLeft, 60);

	GLState state(GLState::BLEND_ON | GLState::TEXTURE_OFF | GLState::DEPTH_OFF); 

	static Vector fontColor;
	fontColor = Vector(0.7f, 0.7f, 0.2f);
	if (timeLeft <= 5)
	{
		fontColor = Vector(0.7f, 0.0f, 0.0f);
	}

	const char *format = "%02i:%02i";
	if (timeLeft < 0) format = "--:--";

	float width = 0.0f;
	if (currentstate == ClientState::StateWait)
	{
		width = GLWFont::instance()->getGameFont()->getWidth(10,
			S3D::formatStringBuffer(format, 
			split.quot,
			split.rem));	
	}
	else 
	{
		width = GLWFont::instance()->getGameFont()->getWidth(20,
			S3D::formatStringBuffer(format, 
			split.quot,
			split.rem));	
	}

	float wHeight = (float) GLViewPort::getHeight();
	float wWidth = (float) GLViewPort::getWidth();
	if (currentstate == ClientState::StateWait)
	{
		static Vector green(0.2f, 0.7f, 0.2f);
		GLWFont::instance()->getGameFont()->draw(
			green, 10, (wWidth/2.0f) - (width / 2), 
			wHeight - 50.0f, 0.0f, 
			S3D::formatStringBuffer(format, 
			split.quot,
			split.rem));	
	}
	else if (showTime_)
	{
		GLWFont::instance()->getGameFont()->draw(
			fontColor, 20, (wWidth/2.0f) - (width / 2),
			wHeight - 50.0f, 0.0f, 
			S3D::formatStringBuffer(format, 
			split.quot,
			split.rem));
	}
}
