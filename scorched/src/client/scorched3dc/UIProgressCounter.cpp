////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <scorched3dc/UIProgressCounter.h>
#include <scorched3dc/UIState.h>
#include <common/Logger.h>

UIProgressThreadCallback::UIProgressThreadCallback(const LangString &op, const float percentage) :
	op_(op), percentage_(percentage)
{
}

UIProgressThreadCallback::~UIProgressThreadCallback()
{
}

void UIProgressThreadCallback::callbackInvoked()
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::DefaultWindow* staticText = static_cast<CEGUI::DefaultWindow*>(wmgr.getWindow("StaticText"));
	if (staticText)
	{
		std::string opStr = LangStringUtil::convertFromLang(op_.c_str());
		staticText->setText(opStr.c_str());
		//Logger::log(S3D::formatStringBuffer("%s\n", opStr.c_str()));
	}
	CEGUI::ProgressBar* progressBar = static_cast<CEGUI::ProgressBar*>(wmgr.getWindow("ProgressBar"));
	if (progressBar)
	{
		progressBar->setProgress(percentage_);
	}
}

ProgressCounter *UIProgressCounter::instance_(0);

ProgressCounter *UIProgressCounter::instance()
{
	if (!instance_) instance_ = new ProgressCounter(new UIProgressCounter());
	return instance_;
}

UIProgressCounter::UIProgressCounter() : lastTime_(0)
{
}

UIProgressCounter::~UIProgressCounter()
{
}

void UIProgressCounter::operationChange(const LangString &op)
{
	progressChange(op, 0.0f);
}

void UIProgressCounter::progressChange(const LangString &op, const float percentage)
{
	time_t currentTime = time(0);
	if (currentTime != lastTime_)
	{
		lastTime_ = currentTime;
		UIProgressThreadCallback *callback = new UIProgressThreadCallback(op, percentage);
		UIState::instance()->getUIThreadCallback().addCallback(callback);
	}
}
