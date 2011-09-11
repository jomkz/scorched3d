////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <dialogs/RulesDialog.h>
#include <dialogs/PlayerDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWWindowManager.h>
#include <target/TargetContainer.h>
#include <client/ClientParams.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <lang/LangResource.h>

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
	okId_ = addWidget(new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 675, 10, 55, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX))->getId();
	motdTab_ = (GLWTab *)
		addWidget(new GLWTab("MoTD", LANG_RESOURCE("MOTD_TAB", "MoTD"), 10, 40, 720, 400));
	rulesTab_ = (GLWTab *)
		addWidget(new GLWTab("Rules", LANG_RESOURCE("RULES_TAB", "Rules"), 10, 40, 720, 400));

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

void RulesDialog::addIcon(const ImageID &imageID)
{
	icon_->setTextureImage(imageID);
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
		++mainitor, ++levelitor)
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

	if (ClientParams::instance()->getConnectAcceptDefaults())
	{
		buttonDown(okId_);
	}
}

void RulesDialog::drawRules()
{
	OptionsScorched &options = ScorchedClient::instance()->getOptionsGame();

	GLState newState(GLState::TEXTURE_OFF | GLState::DEPTH_OFF);

	float top = y_ + h_ - 40.0f;
	float left = x_ + 22.0f;
	Vector yellow(0.3f, 0.3f, 0.3f); // Hmm, thats not yellow

	const char *type = "Annihilate free for all";
	if (options.getTeams() > 1) type = "Annihilate opposing team(s)";

	LANG_RESOURCE_VAR_1(TYPE_LABEL, "TYPE_LABEL", "Type : {0}", type);
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 45.0f, 0.0f,
		TYPE_LABEL);

	LANG_RESOURCE_VAR_1(MOD_LABEL, "MOD_LABEL", "Mod : {0}", 
		ScorchedClient::instance()->getOptionsGame().getMod());
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 75.0f, 0.0f,
		MOD_LABEL);

	LANG_RESOURCE_VAR_1(GAME_TYPE_LABEL, "GAME_TYPE_LABEL", "Game type : {0}", 
		ScorchedClient::instance()->getOptionsGame().getTurnType().getValueAsString());
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 90.0f, 0.0f,
		GAME_TYPE_LABEL);

	LANG_RESOURCE_VAR_1(TEAMS_LABEL, "TEAMS_LABEL", "Teams : {0}", 
		S3D::formatStringBuffer("%i", options.getTeams()));
	LANG_RESOURCE_VAR(TEAMS_NONE, "TEAMS_NONE", "Teams : None");
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 105.0f, 0.0f,
		(options.getTeams() > 1)?TEAMS_LABEL:TEAMS_NONE);

	LANG_RESOURCE_VAR_1(SHOT_TIME_LABEL, "SHOT_TIME_LABEL", "Shot Time : {0}", 
		S3D::formatStringBuffer("%i", options.getShotTime()));
	LANG_RESOURCE_VAR(SHOT_TIME_UNLIMITED, "SHOT_TIME_UNLIMITED", "Shot time : Unlimited");
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 135.0f, 0.0f,
		(options.getShotTime() > 0)?SHOT_TIME_LABEL:SHOT_TIME_UNLIMITED);

	LANG_RESOURCE_VAR_1(BUYING_TIME_LABEL, "BUYING_TIME_LABEL", "Buying Time : {0}", 
		S3D::formatStringBuffer("%i", options.getShotTime()));
	LANG_RESOURCE_VAR(BUYING_TIME_UNLIMITED, "BUYING_TIME_UNLIMITED", "Buying time : Unlimited");
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 150.0f, 0.0f,
		(options.getBuyingTime() > 0)?BUYING_TIME_LABEL:BUYING_TIME_UNLIMITED);
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
