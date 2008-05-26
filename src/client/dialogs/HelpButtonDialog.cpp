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

#include <dialogs/MainMenuDialog.h>
#include <dialogs/HelpButtonDialog.h>
#include <dialogs/SoundDialog.h>
#include <common/Defines.h>
#include <GLW/GLWWindowManager.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLMenu.h>

HelpButtonDialog *HelpButtonDialog::instance_ = 0;

HelpButtonDialog *HelpButtonDialog::instance()
{
	if (!instance_)
	{
		instance_ = new HelpButtonDialog();
	}
	return instance_;
}

HelpButtonDialog::HelpButtonDialog()
{
}

HelpButtonDialog::~HelpButtonDialog()
{
}

HelpButtonDialog::HelpMenu::HelpMenu()
{
	Image *map = 
		ImageFactory::loadImage(
			S3D::getDataFile("data/windows/help.bmp"),
			S3D::getDataFile("data/windows/helpa.bmp"),
			false);
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->
		addMenu("Help", 
			"Launch an external web browser containing the\n"
			"Scorched3D online help.",
			32.0f, 0, this, map,
			GLMenu::eMenuAlignRight);
}

GLTexture &HelpButtonDialog::HelpMenu::getHelpTexture()
{
	if (!helpTexture_.textureValid())
	{
		ImageHandle map = ImageFactory::loadImageHandle(
			S3D::getDataFile("data/windows/help.bmp"),
			S3D::getDataFile("data/windows/helpa.bmp"),
			false);
		helpTexture_.create(map, false);
	}
	return helpTexture_;
}

bool HelpButtonDialog::HelpMenu::getMenuItems(const char* menuName, std::list<GLMenuItem> &result)
{
	result.push_back(GLMenuItem("Show Online Help"));
	return true;
}

void HelpButtonDialog::HelpMenu::menuSelection(const char* menuName, 
	const int position, GLMenuItem &item)
{
	S3D::showURL("http://www.scorched3d.co.uk/wiki");
}

HelpButtonDialog::VolumeMenu::VolumeMenu()
{
	Image *map = ImageFactory::loadImage(
		S3D::getDataFile("data/windows/sound.bmp"),
		S3D::getDataFile("data/windows/sounda.bmp"),
		false);
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->
		addMenu("Volume", 
			"Change the sound and volume settings",
			32.0f, 0, this, map,
			GLMenu::eMenuAlignRight);
}

bool HelpButtonDialog::VolumeMenu::menuOpened(const char* menuName)
{
	GLWWindowManager::instance()->showWindow(
		SoundDialog::instance()->getId());
	return false;
}
