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

#include <dialogs/SaveDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>
#include <lang/LangResource.h>
#include <client/ScorchedClient.h>
#include <common/ChannelManager.h>
#include <common/Defines.h>
#include <time.h>

SaveDialog *SaveDialog::instance_ = 0;

SaveDialog *SaveDialog::instance()
{
	if (!instance_)
	{
		instance_ = new SaveDialog;
	}
	return instance_;
}

SaveDialog::SaveDialog() : 
	GLWWindow("Save", 210.0f, 80.0f, 0,
		"Allows the player to save the game.")
{
	textBox_ = new GLWTextBox(0.0f, 0.0f, 250.0f);
	addWidget(textBox_,
		0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);

	GLWPanel *buttonPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	
	GLWButton *cancelButton = new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX);
	cancelId_ = cancelButton->getId();
	buttonPanel->addWidget(cancelButton, 0, SpaceRight, 10.0f);
	
	GLWButton *okButton = new GLWTextButton(LANG_RESOURCE("SAVE", "Save"), 10, 45, 105, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX);
	okId_ = okButton->getId();
	buttonPanel->addWidget(okButton);
	
	
	buttonPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(buttonPanel, 0, SpaceAll | AlignRight, 10.0f);

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

SaveDialog::~SaveDialog()
{

}

void SaveDialog::display()
{
	GLWWindow::display();

	std::string text = S3D::formatStringBuffer("saved-%i", time(0));
	textBox_->setText(LANG_STRING(text));
}

void SaveDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		if (textBox_->getText()[0])
		{
			std::string saveFile = S3D::formatStringBuffer("%s.s3d", textBox_->getText().c_str());
			//if (ClientSave::saveClient(S3D::getSaveFile(saveFile.c_str())))
			{
				ChannelText text("info", 
					LANG_RESOURCE_1("SAVED_AS", "Saved as \"{0}\"", saveFile));
				ChannelManager::showText(ScorchedClient::instance()->getContext(), text);
			}
			//else
			{
				ChannelText text("info", LANG_RESOURCE("SAVE_FAILED", "Save failed"));
				ChannelManager::showText(ScorchedClient::instance()->getContext(), text);
			}
			GLWWindowManager::instance()->hideWindow(id_);
		}
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}

