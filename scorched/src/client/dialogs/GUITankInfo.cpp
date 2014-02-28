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

#include <dialogs/GUITankInfo.h>
#include <common/DefinesString.h>

GUITankInfo *GUITankInfo::instance()
{
	static GUITankInfo instance_;
	return &instance_;
}

GUITankInfo::GUITankInfo() : 
	window_(0)
{
   create();
   setInvisible();
}

GUITankInfo::~GUITankInfo()
{
	window_ = 0;
}

void GUITankInfo::create()
{
	CEGUI::WindowManager *pWindowManager = CEGUI::WindowManager::getSingletonPtr();
	window_ = pWindowManager->loadLayoutFromFile("TankInfo.layout");
 
	if (window_)
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(window_);
		tankName_ = static_cast<CEGUI::Window*>(window_->getChild("StaticText/Tank"));
		weaponName_ = static_cast<CEGUI::Window*>(window_->getChild("StaticText/Weapon"));
	}
}

void GUITankInfo::setVisible(const CEGUI::String &tankName)
{
	tankName_->setText(tankName);
    window_->setVisible(true);
}

void GUITankInfo::setInvisible()
{
    window_->setVisible(false);
}

void GUITankInfo::setWeaponName(const CEGUI::String &weaponName)
{
	weaponName_->setText(weaponName);
}
