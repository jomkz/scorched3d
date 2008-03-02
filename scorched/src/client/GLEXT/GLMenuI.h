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


// GLMenuI.h: interface for the GLMenuI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLMENUI_H__7171A46C_8265_4A9F_A984_889EF2547CB1__INCLUDED_)
#define AFX_GLMENUI_H__7171A46C_8265_4A9F_A984_889EF2547CB1__INCLUDED_

#include <list>
#include <string>
#include <GLW/GLWToolTip.h>

class GLTexture;
class GLMenuItem
{
public:
	GLMenuItem(const char *text, 
		ToolTip *tooltip = 0, bool selected = false, 
		GLTexture *texture = 0,
		void *userData = 0);

	const char *getText() { return menuText_.c_str(); }
	ToolTip *getToolTip() { return tip_; }
	GLTexture *getTexture() { return texture_; }
	bool getSelected() { return selected_; }
	void *getUserData() { return userData_; }

protected:
	std::string menuText_;
	ToolTip *tip_;
	GLTexture *texture_;
	bool selected_;
	void *userData_;
};

class GLMenuI  
{
public:
	virtual ~GLMenuI();

	virtual void menuSelection(const char* menuName, const int position, GLMenuItem &item);
	virtual const char *getMenuText(const char* menuName);
	virtual bool getEnabled(const char* menuName);
	virtual bool getMenuItems(const char* menuName, std::list<GLMenuItem> &result);
	virtual bool menuOpened(const char* menuName);
};

#endif // !defined(AFX_GLMENUI_H__7171A46C_8265_4A9F_A984_889EF2547CB1__INCLUDED_)
