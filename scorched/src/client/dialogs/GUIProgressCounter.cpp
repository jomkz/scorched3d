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

#include <scorched3dc/ScorchedUI.h>
#include <dialogs/GUIProgressCounter.h>
#include <scorched3dc/UIState.h>
#include <common/Logger.h>

GUIProgress *GUIProgress::instance()
{
	static GUIProgress instance_;
	return &instance_;
}

GUIProgress::GUIProgress() : 
	window_(0), totalTime_(0.0f), frameCount_(0)
{
   create();
   setVisible(false);
}

GUIProgress::~GUIProgress()
{
	window_ = 0;
}

void GUIProgress::create()
{
	CEGUI::WindowManager *pWindowManager = CEGUI::WindowManager::getSingletonPtr();
	window_ = pWindowManager->loadLayoutFromFile("Progress.layout");
 
	if (window_)
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(window_);
		staticText_ = window_;
		progressBar_ = static_cast<CEGUI::ProgressBar*>(window_->getChild("ProgressBar"));
	}
}

void GUIProgress::setVisible(bool visible)
{
	staticText_->setText("");
	progressBar_->setProgress(0.0f);
    window_->setVisible(visible);
}
 
bool GUIProgress::isVisible()
{
    return window_->isVisible();
}

GUIProgressThreadCallback::GUIProgressThreadCallback(const LangString &op, const float percentage) :
	op_(op), percentage_(percentage)
{
}

GUIProgressThreadCallback::~GUIProgressThreadCallback()
{
}

void GUIProgressThreadCallback::callbackInvoked()
{
	if (ScorchedUI::instance()->getUIState().getState() != UIState::StateProgress)
	{
		return;
	}

	CEGUI::Window *staticText = GUIProgress::instance()->getStaticText();
	if (staticText)
	{
		std::string opStr = LangStringUtil::convertFromLang(op_.c_str());
		staticText->setText(opStr.c_str());
	}
	CEGUI::ProgressBar *progressBar = GUIProgress::instance()->getProgressBar();
	if (progressBar)
	{
		progressBar->setProgress(percentage_);
	}
	delete this;
}

ProgressCounter *GUIProgressCounter::instance_(0);

ProgressCounter *GUIProgressCounter::instance()
{
	if (!instance_) instance_ = new ProgressCounter(new GUIProgressCounter());
	return instance_;
}

GUIProgressCounter::GUIProgressCounter() : 
	lastTime_(0), threadCallback_(false)
{
}

GUIProgressCounter::~GUIProgressCounter()
{
}

void GUIProgressCounter::updateProgress()
{
	threadCallback_.processCallbacks();
}

void GUIProgressCounter::operationChange(const LangString &op)
{
	progressChange(op, 0.0f);
}

void GUIProgressCounter::progressChange(const LangString &op, const float percentage)
{
	time_t currentTime = time(0);
	if (currentTime != lastTime_)
	{
		lastTime_ = currentTime;
		GUIProgressThreadCallback *callback = new GUIProgressThreadCallback(op, percentage);
		threadCallback_.addCallback(callback);
	}
}
