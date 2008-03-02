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

#if !defined(__INCLUDE_ModSubSelectDialogh_INCLUDE__)
#define __INCLUDE_ModSubSelectDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWButton.h>
#include <GLW/GLWIconList.h>
#include <GLW/GLWIcon.h>
#include <common/ToolTip.h>
#include <engine/ModInfo.h>

class GLWIconListSubModItem : public GLWIconListItem
{
public:
	GLWIconListSubModItem(ModInfo::MenuEntry &modInfoEntry);
	virtual ~GLWIconListSubModItem();

	ModInfo::MenuEntry &getModInfoEntry() { return modInfoEntry_; }

	// GLWIconListItem
	virtual void draw(float x, float y, float w);

protected:
	ModInfo::MenuEntry modInfoEntry_;
	GLWIcon icon_;
	ToolTip tip_;
};

class ModSubSelectDialog : 
	public GLWWindow,
	public GLWButtonI,
	public GLWIconListI
{
public:
	static ModSubSelectDialog *instance();

	void setModInfo(ModInfo &modInfo);

	// GLWWindow
	virtual void display();

	// GLWButtonI
	virtual void buttonDown(unsigned int id);

	// GLWIconListI
	virtual void selected(unsigned int id, int position);
	virtual void chosen(unsigned int id, int position);

protected:
	static ModSubSelectDialog *instance_;

	unsigned int okId_, cancelId_;
	GLWIconList *iconList_;
	ModInfo modInfo_;

private:
	ModSubSelectDialog();
	virtual ~ModSubSelectDialog();

};

#endif
