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

#include <dialogs/KibitzingDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>

KibitzingDialog::KibitzingDialog() : 
	GLWWindow("Kibitzing", 280.0f, 80.0f, 0,
		"Shows a message saying\n\"No kibitzing, please.\"")
{
	addWidget(new GLWLabel(10, 40, LANG_RESOURCE("No kibitzing, please.")));

	okId_ = addWidget(new GLWTextButton(LANG_RESOURCE("Ok"), 215, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCancel | 
		GLWButton::ButtonFlagCenterX))->getId();
}

KibitzingDialog::~KibitzingDialog()
{

}

void KibitzingDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}
