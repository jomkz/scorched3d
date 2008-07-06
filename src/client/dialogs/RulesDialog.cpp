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

#include <dialogs/RulesDialog.h>
#include <dialogs/PlayerDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWWindowManager.h>
#include <tank/TankContainer.h>
#include <client/ClientParams.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <client/ScorchedClient.h>

RulesDialog *RulesDialog::instance_ = 0;

RulesDialog *RulesDialog::instance()
{
	if (!instance_)
	{
		instance_ = new RulesDialog();
	}
	return instance_;
}

RulesDialog::RulesDialog() : 
	GLWWindow("Rules", 0.0f, 0.0f, 740.0f, 480.0f, eSmallTitle,
		"Shows the game rules for the game\n"
		"in progress.")
{
	needCentered_ = true;
	okId_ = addWidget(new GLWTextButton("Ok", 675, 10, 55, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX))->getId();
	motdTab_ = (GLWTab *)
		addWidget(new GLWTab("MoTD", 10, 40, 720, 400));
	rulesTab_ = (GLWTab *)
		addWidget(new GLWTab("Rules", 10, 40, 720, 400));

	motdList_ = (GLWListView *) motdTab_->
		addWidget(new GLWListView(10, 10, 700, 325, 100));
	rulesList_ = (GLWListView *) rulesTab_->
		addWidget(new GLWListView(10, 10, 700, 215, 500));
	icon_ = (GLWIcon *) motdTab_->
		addWidget(new GLWIcon(165, 340, 390, 50));
}

RulesDialog::~RulesDialog()
{
}

void RulesDialog::addIcon(GLTexture *texture)
{
	delete icon_->getTexture();
	icon_->setTexture(texture);
}

void RulesDialog::addMOTD(const char *text)
{
	motdTab_->setDepressed();
	motdList_->clear();
	rulesList_->clear();

	// Add all the server rules
	std::list<OptionEntry *> &leveloptions = 
		ScorchedClient::instance()->getOptionsGame().getLevelOptions().getOptions();
	std::list<OptionEntry *> &mainoptions =
		ScorchedClient::instance()->getOptionsGame().getMainOptions().getOptions();
	std::list<OptionEntry *>::iterator mainitor;
	std::list<OptionEntry *>::iterator levelitor;
	for (mainitor = mainoptions.begin(), levelitor = leveloptions.begin();
		mainitor != mainoptions.end() && levelitor != leveloptions.end();
		mainitor++, levelitor++)
	{
		OptionEntry *mainentry = (*mainitor);
		OptionEntry *levelentry = (*levelitor);
		OptionEntry *entry = mainentry;
		if (levelentry->isChangedValue()) entry = levelentry;

		rulesList_->addLine(
			S3D::formatStringBuffer("%s = %s%s",
				entry->getName(),
				entry->getValueAsString(),
				((entry == levelentry)?" L":(entry->isDefaultValue()?"":"*"))));
	}

	// Add single or multiple lines
	char *found = (char *) strchr(text, '\n');
	char *start = (char *) text;
	if (found)
	{
		while (found)
		{
			*found = '\0';
			motdList_->addLine(start);
			start = found;
			start++;
			*found = '\n';

			found = strchr(start, '\n');
		}
		if (start[0] != '\0')
		{
			motdList_->addLine(start);
		}
	}
	else
	{
		motdList_->addLine(text);
	}
}

void RulesDialog::draw()
{
	GLWWindow::draw();

	if (rulesTab_->getDepressed())
	{
		drawRules();
	}
}

void RulesDialog::drawRules()
{
	OptionsTransient &optionsT = ScorchedClient::instance()->getOptionsTransient();
	OptionsScorched &options = ScorchedClient::instance()->getOptionsGame();

	GLState newState(GLState::TEXTURE_OFF | GLState::DEPTH_OFF);

	float top = y_ + h_ - 40.0f;
	float left = x_ + 22.0f;
	Vector yellow(0.3f, 0.3f, 0.3f); // Hmm, thats not yellow

	const char *type = "Annihilate free for all";
	if (options.getTeams() > 1) type = "Annihilate opposing team(s)";

	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 45.0f, 0.0f,
		S3D::formatStringBuffer("Type : %s", 
		type));


	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 75.0f, 0.0f,
		S3D::formatStringBuffer("Mod : %s", 
		ScorchedClient::instance()->getOptionsGame().getMod()));
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 90.0f, 0.0f,
		S3D::formatStringBuffer("Game type : %s", 
		ScorchedClient::instance()->getOptionsTransient().getGameType()));
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 105.0f, 0.0f,
		S3D::formatStringBuffer(((options.getTeams() > 1)?"Teams : %i":"Teams : None"),
		options.getTeams()));
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 135.0f, 0.0f,
		S3D::formatStringBuffer(((options.getShotTime() > 0)?"Shot time : %i (s)":"Shot time : Unlimited"),
		options.getShotTime()));
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 150.0f, 0.0f,
		S3D::formatStringBuffer(((options.getBuyingTime() > 0)?"Buying time : %i (s)":"Buying time : Unlimited"),
		options.getShotTime()));
}

void RulesDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);	
	}
}

void RulesDialog::display()
{
	addMOTD(ScorchedClient::instance()->getOptionsGame().getMOTD());

	GLWWindow::display();
}

void RulesDialog::hide()
{
	GLWWindow::hide();
	GLWWindowManager::instance()->showWindow(
		PlayerDialog::instance()->getId());	
}
