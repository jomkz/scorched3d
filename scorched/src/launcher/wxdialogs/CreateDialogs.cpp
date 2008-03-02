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

#include <wxdialogs/MainDialog.h>
#include <server/ServerMain.h>
#include <common/OptionsGame.h>
#include <wx/wx.h>
#include <wx/image.h>
#include <locale.h>

bool wxWindowInit = false;
class ScorchedApp: public wxApp
{
    bool OnInit();
};     

bool ScorchedApp::OnInit()
{
	if (setlocale(LC_ALL, "C") == 0)
	{
		S3D::dialogMessage(
			"ScorchedApp",
			"Warning: Failed to set wx locale");
	}

	wxImage::AddHandler(new wxICOHandler);
	wxImage::AddHandler(new wxGIFHandler);

	// Show the launcher dialogs
	showMainDialog();
	SetTopWindow(getMainDialog());

	wxWindowInit = true;
	return TRUE;
}

IMPLEMENT_APP_NO_MAIN(ScorchedApp);
IMPLEMENT_WX_THEME_SUPPORT
