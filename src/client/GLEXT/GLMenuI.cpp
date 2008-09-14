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

#include <GLEXT/GLMenuI.h>

GLMenuItem::GLMenuItem(const LangString &text, 
	ToolTip *tooltip, 
	bool selected,
	GLTexture *texture,
	void *userData) : 
	menuText_(text), tip_(tooltip), 
	selected_(selected), texture_(texture),
	userData_(userData), seperator_(false)
{
}

GLMenuI::~GLMenuI()
{

}

LangString *GLMenuI::getMenuText(const char* menuName)
{
	return 0;
}

bool GLMenuI::getMenuItems(const char* menuName, std::list<GLMenuItem> &result)
{
	return false;
}

bool GLMenuI::getEnabled(const char* menuName)
{
	return true;
}

void GLMenuI::menuSelection(const char* menuName, const int position, GLMenuItem &item)
{

}

bool GLMenuI::menuOpened(const char* menuName)
{
	return true;
}

const char *GLMenuI::getMenuToolTip(const char* menuName)
{
	return 0;
}
