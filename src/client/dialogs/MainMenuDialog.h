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


// MainMenuDialog.h: interface for the MainMenuDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINMENU_H__F1A639D4_3F3E_484A_BBEB_1E1585B8D7B9__INCLUDED_)
#define AFX_MAINMENU_H__F1A639D4_3F3E_484A_BBEB_1E1585B8D7B9__INCLUDED_

#include <GLEXT/GLMenu.h>

class MainMenuDialog : public GLMenu
{
public:
	static MainMenuDialog* instance();

protected:
	static MainMenuDialog* instance_;

private:
	MainMenuDialog();
	virtual ~MainMenuDialog();

};

#endif // !defined(AFX_MAINMENU_H__F1A639D4_3F3E_484A_BBEB_1E1585B8D7B9__INCLUDED_)
