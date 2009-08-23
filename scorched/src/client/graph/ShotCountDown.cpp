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
#include <tank/TankContainer.h>
#include <lang/LangResource.h>

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
	show_(false)
{
}

ShotCountDown::~ShotCountDown()
{
}

void ShotCountDown::show(fixed timer, TimerType type, unsigned int playerId)
{
	show_ = true;
	timer_ = timer;
	type_ = type;
	playerId_ = playerId;
}

void ShotCountDown::draw(const unsigned currentstate)
{
	if (!show_) return;
	show_ = false;

	GLState state(GLState::BLEND_ON | GLState::TEXTURE_OFF | GLState::DEPTH_OFF); 

	static Vector yellow(0.85f, 0.85f, 0.3f);
	static Vector darkYellow(0.7f, 0.7f, 0.2f);
	static Vector red(0.7f, 0.0f, 0.0f);

	std::string str;
	Vector *fontColor = 0;
	if (timer_ <= 5)
	{
		fontColor = &red;

		str = S3D::formatStringBuffer("%.1f", 
			timer_.asFloat());		
	}
	else
	{
		fontColor = &yellow;

		int timeLeft = timer_.asInt();
		div_t split = div(timeLeft, 60);
		str = S3D::formatStringBuffer("%02i:%02i", 
			split.quot,
			split.rem);
	}

	float width = GLWFont::instance()->getGameFont()->getWidth(20, str);	
	float wHeight = (float) GLViewPort::getHeight();
	float wWidth = (float) GLViewPort::getWidth();
	GLWFont::instance()->getGameFont()->draw(
		*fontColor, 20, (wWidth/2.0f) - (width / 2),
		wHeight - 50.0f, 0.0f, 
		str);

	if (playerId_ != 0)
	{
		Tank *tank = ScorchedClient::instance()->getTankContainer().getTankById(playerId_);
		if (tank)
		{
			float playerWidth = GLWFont::instance()->getGameFont()->getWidth(14, tank->getTargetName());
			GLWFont::instance()->getGameFont()->drawWidthRhs(
				200.0f, darkYellow, 14, (wWidth/2.0f) - (width / 2) - playerWidth - 10.0f,
				wHeight - 47.0f, 0.0f, 
				tank->getTargetName());

			LANG_RESOURCE_VAR(buyingString, "BUYING", "Buying");
			LANG_RESOURCE_VAR(playingString, "PLAYING", "Playing");

			GLWFont::instance()->getGameFont()->drawWidthRhs(
				200.0f, darkYellow, 14, (wWidth/2.0f) + (width / 2) + 10.0f,
				wHeight - 47.0f, 0.0f, 
				type_==eBuying?buyingString:playingString);
		}
	}
}
