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
#include <dialogs/GUILogo.h>

GUILogo *GUILogo::instance()
{
	static GUILogo instance_;
	return &instance_;
}

GUILogo::GUILogo() : 
	window_(0)
{
   create();
   setVisible(false);
}

GUILogo::~GUILogo()
{
	window_ = 0;
}

void GUILogo::create()
{
	CEGUI::WindowManager *pWindowManager = CEGUI::WindowManager::getSingletonPtr();
	window_ = pWindowManager->loadLayoutFromFile("Logo.layout");
 
	if (window_)
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(window_);
	}
}

void GUILogo::setVisible(bool visible)
{
    window_->setVisible(visible);
}
 
bool GUILogo::isVisible()
{
    return window_->isVisible();
}
