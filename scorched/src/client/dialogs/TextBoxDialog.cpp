////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <dialogs/TextBoxDialog.h>
#include <GLW/GLWWindowManager.h>

TextBoxDialog *TextBoxDialog::instance_ = 0;

TextBoxDialog *TextBoxDialog::instance()
{
	if (!instance_)
	{
		instance_ = new TextBoxDialog;
	}
	return instance_;
}

TextBoxDialog::TextBoxDialog() : 
	GLWWindow("MsgBox", 210.0f, 150.0f, eHideName, "")
{
	GLWPanel *topPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);

	message_ = new GLWTextBox(0.0f, 0.0f, 300.0f, LANG_STRING(""));
	topPanel->addWidget(message_, 0, SpaceTop | SpaceLeft | SpaceRight | AlignCenterLeftRight, 10.0f);
	topPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(topPanel);

	GLWPanel *botPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	cancelButton_ = (GLWTextButton *) 
		botPanel->addWidget(new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | 
		GLWButton::ButtonFlagCenterX), 0, SpaceAll | AlignRight, 10.0f);
	okButton_ = (GLWTextButton *) 
		botPanel->addWidget(new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 95, 10, 105, this, 
		GLWButton::ButtonFlagOk | 
		GLWButton::ButtonFlagCenterX), 0, SpaceAll | AlignRight, 10.0f);
	botPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(botPanel);

	windowLevel_ = 1000;

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

TextBoxDialog::~TextBoxDialog()
{

}

void TextBoxDialog::show(const LangString &message)
{
	instance()->message_->setText(message);
	instance()->message_->calcWidth();
	instance()->layout();

	GLWWindowManager::instance()->showWindow(instance()->getId());
}

void TextBoxDialog::buttonDown(unsigned int id)
{
	if (id == okButton_->getId())
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}

void TextBoxDialog::mouseDown(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseDown(button, x, y, skipRest);
	skipRest = true;
}

void TextBoxDialog::mouseUp(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseUp(button, x,y, skipRest);
	skipRest = true;
}

void TextBoxDialog::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	GLWWindow::keyDown(buffer, keyState, history, hisCount, skipRest);
	skipRest = true;
}

void TextBoxDialog::mouseWheel(float x, float y, float z, bool &skipRest)
{
	GLWWindow::mouseWheel(x, y, z, skipRest);
	skipRest = true;
}

