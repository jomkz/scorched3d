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

#include <dialogs/TipDialog.h>
#include <graph/OptionsDisplay.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWWindowManager.h>

TipDialog *TipDialog::instance_ = 0;

TipDialog *TipDialog::instance()
{
	if (!instance_)
	{
		instance_ = new TipDialog;
	}
	return instance_;
}

TipDialog::TipDialog() :
	GLWWindow("Tooltip Settings", 10.0f, 10.0f, 300.0f, 70.0f, eSmallTitle,
		"Allow the user to change Tooltip settings")
{
	needCentered_ = true;

	helpBox_ = new GLWCheckBoxText(0.0f, 0.0f, "Show Help Tooltips");
	addWidget(helpBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	infoBox_ = new GLWCheckBoxText(0.0f, 0.0f, "Show Info Tooltips");
	addWidget(infoBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);

	GLWPanel *buttonPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	GLWButton *cancelButton = new GLWTextButton("Cancel", 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX);
	cancelId_ = cancelButton->getId();
	buttonPanel->addWidget(cancelButton, 0, SpaceRight, 10.0f);
	GLWButton *okButton = new GLWTextButton("Ok", 235, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX);
	okId_ = okButton->getId();
	buttonPanel->addWidget(okButton);
	buttonPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(buttonPanel, 0, SpaceAll, 10.0f);

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

TipDialog::~TipDialog()
{
}

void TipDialog::display()
{
	GLWWindow::display();

	helpBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getShowContextHelp());
	infoBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getShowContextInfo());
}

void TipDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		OptionsDisplay::instance()->getShowContextHelpEntry().setValue(
			helpBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getShowContextInfoEntry().setValue(
			infoBox_->getCheckBox().getState());
	}
	GLWWindowManager::instance()->hideWindow(getId());
}
