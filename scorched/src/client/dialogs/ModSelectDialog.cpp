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

#include <dialogs/ModSelectDialog.h>
#include <dialogs/ModSubSelectDialog.h>
#include <dialogs/SettingsSelectDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <graph/TextureStore.h>
#include <engine/ModDirs.h>
#include <common/Defines.h>

GLWIconListModItem::GLWIconListModItem(ModInfo &modInfo) :
	modInfo_(modInfo), 
	tip_(ToolTip::ToolTipHelp, modInfo.getName(), modInfo.getDescription()),
	icon_(0.0f, 0.0f, 40.0f, 40.0f)
{
	if (S3D::fileExists(modInfo_.getIcon()))
	{
		GLTexture *texture = TextureStore::instance()->loadTexture(
			modInfo_.getIcon());
		icon_.setTexture(texture);
	}
}

GLWIconListModItem::~GLWIconListModItem()
{
}

void GLWIconListModItem::draw(float x, float y, float w)
{
	icon_.setX(x + 2.0f);
	icon_.setY(y + 2.0f);
	icon_.draw();

	GLWToolTip::instance()->addToolTip(&tip_, 
		GLWTranslate::getPosX() + x, 
		GLWTranslate::getPosY() + y, w, 50.0f);

	GLWFont::instance()->getGameFont()->draw(
		GLWFont::widgetFontColor, 
		12.0f, x + 50.0f, y + 25.0f, 0.0f, 
		S3D::formatStringBuffer("Mod : %s", modInfo_.getName()));
	GLWFont::instance()->getGameFont()->drawWidth(
		w - 50.0f,
		GLWFont::widgetFontColor, 
		8.0f, x + 50.0f, y + 12.0f, 0.0f, 
		modInfo_.getShortDescription());
}

ModSelectDialog *ModSelectDialog::instance_ = 0;

ModSelectDialog *ModSelectDialog::instance()
{
	if (!instance_)
	{
		instance_ = new ModSelectDialog;
	}
	return instance_;
}

ModSelectDialog::ModSelectDialog() : 
	GLWWindow("", 300.0f, 410.0f, 0, "")
{
	iconList_ = new GLWIconList(10.0f, 40.0f, 280.0f, 360.0f, 50.0f);
	addWidget(iconList_);

	okId_ = addWidget(new GLWTextButton("Ok", 235, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX))->getId();
	cancelId_ = addWidget(new GLWTextButton("Cancel", 120, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX))->getId();

	iconList_->setHandler(this);
}

ModSelectDialog::~ModSelectDialog()
{

}

void ModSelectDialog::display()
{
	iconList_->clear();

	ModDirs modDirs;
	if (!modDirs.loadModDirs())
	{
		S3D::dialogExit("ModSelectDialog", "Failed to load mod dirs");	
	}

	std::list<ModInfo>::iterator itor;
	for (itor = modDirs.getDirs().begin();
		itor != modDirs.getDirs().end();
		itor++)
	{
		ModInfo &info = (*itor);
		if (!info.getMenuEntries().empty())
		{
			GLWIconListModItem *item = new GLWIconListModItem(info);
			iconList_->addItem(item);
		}
	}

	// Add the info that represents a custom game
	{
		ModInfo customInfo("Custom");
		customInfo.parse(S3D::getDataFile("data/custominfo.xml"));
		GLWIconListModItem *item = new GLWIconListModItem(customInfo);
		iconList_->addItem(item);
	}
}

void ModSelectDialog::selected(unsigned int id, int position)
{
}

void ModSelectDialog::chosen(unsigned int id, int position)
{
	buttonDown(okId_);
}

void ModSelectDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		GLWIconListModItem *selected = 
			(GLWIconListModItem *) iconList_->getSelected();
		if (selected)
		{
			if (0 == strcmp("Custom", selected->getModInfo().getName()))
			{
				GLWWindowManager::instance()->showWindow(
					SettingsSelectDialog::instance()->getId());
			}
			else
			{
				ModSubSelectDialog::instance()->setModInfo(selected->getModInfo());
				GLWWindowManager::instance()->showWindow(
					ModSubSelectDialog::instance()->getId());
			}
		}
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}
