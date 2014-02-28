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

#include <dialogs/GUIFrameTimer.h>
#include <common/DefinesString.h>

GUIFrameTimer *GUIFrameTimer::instance()
{
	static GUIFrameTimer instance_;
	return &instance_;
}

GUIFrameTimer::GUIFrameTimer() : 
	window_(0), totalTime_(0.0f), frameCount_(0)
{
   create();
   setVisible(false);
}

GUIFrameTimer::~GUIFrameTimer()
{
	window_ = 0;
}

void GUIFrameTimer::create()
{
	CEGUI::WindowManager *pWindowManager = CEGUI::WindowManager::getSingletonPtr();
	window_ = pWindowManager->loadLayoutFromFile("FrameTimer.layout");
 
	if (window_)
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(window_);
		staticText_ = static_cast<CEGUI::Window*>(window_->getChild("StaticText"));
	}
}

void GUIFrameTimer::setVisible(bool visible)
{
	staticText_->setText("0.0");
    window_->setVisible(visible);
}
 
bool GUIFrameTimer::isVisible()
{
    return window_->isVisible();
}

void GUIFrameTimer::simulate(float frameTime)
{
	totalTime_ += frameTime;
	frameCount_++;
	if (totalTime_ > 5.0f)
	{
		float fps = float(frameCount_) / totalTime_;
		staticText_->setText(S3D::formatStringBuffer("%.2f", fps));
		totalTime_ = 0.0f;
		frameCount_ = 0;
	}
}
