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

#include <dialogs/NetworkSelectDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <graph/TextureStore.h>
#include <serverbrowser/ServerBrowser.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <client/ClientMain.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>

struct ColumnInfo
{
	GLWIconTable::Column col;
	const char *dataName;
};
static ColumnInfo gamescols[] = 
{
	GLWIconTable::Column("", 60), "",
	GLWIconTable::Column("Server Name", 255), "servername",
	GLWIconTable::Column("Plyrs", 60), "noplayers",
	GLWIconTable::Column("Round", 55), "round",
	GLWIconTable::Column("Mod", 95), "mod",
	GLWIconTable::Column("Game Type", 200), "gametype"
};
static ColumnInfo playerscols[] =
{
	GLWIconTable::Column("Player", 270), "pn",
	GLWIconTable::Column("Score", 260), "ps",
	GLWIconTable::Column("Time", 140), "pt",
	GLWIconTable::Column("Real", 60), "pa"
};

NetworkSelectDialog *NetworkSelectDialog::instance_ = 0;

NetworkSelectDialog *NetworkSelectDialog::instance()
{
	if (!instance_)
	{
		instance_ = new NetworkSelectDialog;
	}
	return instance_;
}

NetworkSelectDialog::NetworkSelectDialog() : 
	GLWWindow("", 780.0f, 560.0f, 0, ""),
	totalTime_(0.0f), invalidateId_(0),
	okTex_(0), questionTex_(0),
	warningTex_(0), noentryTex_(0),
	tankTex_(0)
{
	std::list<GLWIconTable::Column> gamescolumns, playerscolumns;
	for (int i=0; i<sizeof(gamescols)/sizeof(ColumnInfo); i++)
	{
		gamescolumns.push_back(gamescols[i].col);
	}
	for (int i=0; i<sizeof(playerscols)/sizeof(ColumnInfo); i++)
	{
		playerscolumns.push_back(playerscols[i].col);
	}

	gamesIconTable_ = new GLWIconTable(10.0f, 165.0f, 760.0f, 350.0f, &gamescolumns, 20.0f);
	addWidget(gamesIconTable_);
	gamesIconTable_->setHandler(this);

	playersIconTable_ = new GLWIconTable(10.0f, 45.0f, 760.0f, 110.0f, &playerscolumns, 20.0f);
	addWidget(playersIconTable_);
	playersIconTable_->setHandler(this);

	ok_ = (GLWTextButton *) addWidget(
		new GLWTextButton(LANG_RESOURCE("Join Game"), 640, 10, 130, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX));
	cancelId_ = addWidget(
		new GLWTextButton(LANG_RESOURCE("Cancel"), 530, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX))->getId();
	refresh_ = (GLWTextButton *) addWidget(
		new GLWTextButton(LANG_RESOURCE("Refresh List"), 175, 10, 150, this, 
		GLWButton::ButtonFlagCenterX));
	favourites_ = (GLWTextButton *) addWidget(
		new GLWTextButton(LANG_RESOURCE("Add Favourite"), 10, 10, 155, this, 
		GLWButton::ButtonFlagCenterX));

	ipaddress_ = (GLWTextBox *) addWidget(
		new GLWTextBox(210.0f, 525.0f, 300.0));
	addWidget(new GLWLabel(75.0f, 525.0f, LANG_RESOURCE("Connect To :")));
	ipaddress_->setHandler(this);

	refreshType_ = (GLWDropDownText *) addWidget(
		new GLWDropDownText(530.0f, 525.0f, 150.0f));
	refreshType_->addText("Internet");
	refreshType_->addText("LAN");
	refreshType_->addText("Favourites");
	refreshType_->setCurrentText("Internet");
	refreshType_->setHandler(this);

	ipaddress_->setCurrent();
}

NetworkSelectDialog::~NetworkSelectDialog()
{

}

void NetworkSelectDialog::simulate(float frameTime)
{
	GLWWindow::simulate(frameTime);

	totalTime_ += frameTime;
	if (totalTime_ > 1.0f)
	{
		updateTable();
	}
}

void NetworkSelectDialog::drawIcon(GLTexture *tex, float &x, float y, const char *message)
{
	GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON);
	glColor3f(1.0f, 1.0f, 1.0f);
	tex->draw();

	float w = 18.0f;
	float h = 18.0f;
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x, y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x + w, y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x + w, y + h);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x, y + h);
	glEnd();

	if (GLWToolTip::instance()->addToolTip(&colToolTip_, 
		GLWTranslate::getPosX() + x, 
		GLWTranslate::getPosY() + y, 
		w, h))
	{
		colToolTip_.setText(ToolTip::ToolTipInfo, "Status Icon", message);
	}

	x += w;
}

GLTexture *NetworkSelectDialog::getTexture(int row, const char *&message)
{
	std::string pversion =
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "protocolversion");
	std::string version =
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "version");
	if (!serverCompatable(pversion, version))
	{
		message = "Incompatible version.";
		return noentryTex_;
	}

	std::string clients = 
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "noplayers");
	std::string maxclients = 
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "maxplayers");
	if (clients.size() > 0 &&
		0 == strcmp(clients.c_str(), maxclients.c_str()))
	{
		message = "Server is full.";
		return exclaimTex_;
	}
	
	std::string state = 
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "state");
	if (0 == strcmp(state.c_str(), "Waiting"))
	{
		message = "Game has not started.";
		return warningTex_;
	}
	if (0 == strcmp(state.c_str(), "Started"))
	{
		message = "Game in progress and spaces on server.";
		return okTex_;
	}

	message = "Cannot contact server.";
	return questionTex_;
}

void NetworkSelectDialog::drawColumn(unsigned int id, int row, int col,
	float x, float y, float w)
{
	if (id == gamesIconTable_->getId()) drawColumnGames(id, row, col, x, y, w);
	else if (id == playersIconTable_->getId()) drawColumnPlayers(id, row, col, x, y, w);
}

void NetworkSelectDialog::drawColumnGames(unsigned int id, int row, int col, 
	float x, float y, float w)
{
	if (row < 0 || row >= ServerBrowser::instance()->getServerList().getNoEntries())
	{
		return;
	}

	std::string value, tipValue;
	if (col == 0)
	{
		if (!okTex_)
		{
			okTex_ = TextureStore::instance()->loadTexture(
				S3D::getDataFile("data/windows/ok.bmp"),
				S3D::getDataFile("data/windows/mask.bmp"));
			questionTex_ = TextureStore::instance()->loadTexture(
				S3D::getDataFile("data/windows/question.bmp"),
				S3D::getDataFile("data/windows/mask.bmp"));
			warningTex_ = TextureStore::instance()->loadTexture(
				S3D::getDataFile("data/windows/warn.bmp"),
				S3D::getDataFile("data/windows/mask.bmp"));
			noentryTex_ = TextureStore::instance()->loadTexture(
				S3D::getDataFile("data/windows/noentry.bmp"),
				S3D::getDataFile("data/windows/mask.bmp"));
			exclaimTex_ = TextureStore::instance()->loadTexture(
				S3D::getDataFile("data/windows/exclaim.bmp"),
				S3D::getDataFile("data/windows/mask.bmp"));
			keyTex_ = TextureStore::instance()->loadTexture(
				S3D::getDataFile("data/windows/key.bmp"),
				S3D::getDataFile("data/windows/keya.bmp"),
				true);
			cogTex_ = TextureStore::instance()->loadTexture(
				S3D::getDataFile("data/windows/cog.bmp"),
				S3D::getDataFile("data/windows/coga.bmp"),
				true);
			tankTex_ = TextureStore::instance()->loadTexture(
				S3D::getDataFile("data/windows/tank2s.bmp"));
		}

		const char *message = "None";
		GLTexture *tex = getTexture(row, message);
		drawIcon(tex, x, y, message);

		std::string key = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "password");
		if (0 == strcmp(key.c_str(), "On"))
		{
			drawIcon(keyTex_, x, y, "Password protected.");
		}

		std::string officialStr = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "type");
		if (officialStr == "official")
		{
			drawIcon(tankTex_, x, y, "An offical server.");
		}
		else if (officialStr == "mod")
		{
			drawIcon(cogTex_, x, y, "Home of mod server.");
		}
	}
	else if (col == 2)
	{
		std::string clients = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "noplayers");
		std::string maxclients = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "maxplayers");
		std::string compplayers = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "compplayers");

		std::string name;
		if (compplayers.c_str()[0])
		{
			name = S3D::formatStringBuffer("%s/%s (%i)", clients.c_str(), maxclients.c_str(),
				(atoi(clients.c_str()) - atoi(compplayers.c_str())));
		}
		else
		{
			name = S3D::formatStringBuffer("%s/%s (?)", clients.c_str(), maxclients.c_str());
		}

		std::string message = name;
		int noplayers = atoi(clients.c_str());
		for (int i=0; i<noplayers; i++)
		{
			std::string pn = 
				ServerBrowser::instance()->getServerList().getEntryValue(
					row, S3D::formatStringBuffer("pn%i", i));
			std::string pa = 
				ServerBrowser::instance()->getServerList().getEntryValue(
					row, S3D::formatStringBuffer("pa%i", i));

			message.append(S3D::formatStringBuffer("\n%i: %s: %s", 
				i, pa.c_str(), pn.c_str()));
		}

		value = S3D::formatStringBuffer("%s", name.c_str());
		tipValue = S3D::formatStringBuffer("%s", message.c_str());
	}
	else
	{
		tipValue = value = ServerBrowser::instance()->getServerList().getEntryValue(row, gamescols[col].dataName);
		if (value[0] == '\0' && col == 1)
		{
			tipValue = value = ServerBrowser::instance()->getServerList().getEntryValue(row, "address");
		}
	}

	if (!value.empty())
	{
		Vector color(0.3f, 0.3f, 0.3f);
		GLWFont::instance()->getGameFont()->drawWidth(w, 
			color, 
			10.0f, x + 3.0f, y + 5.0f, 0.0f, value);

		if (!tipValue.empty())
		{
			if (GLWToolTip::instance()->addToolTip(&colToolTip_, 
				GLWTranslate::getPosX() + x, 
				GLWTranslate::getPosY() + y, 
				w, 20.0f))
			{
				colToolTip_.setText(ToolTip::ToolTipInfo, gamescols[col].col.name.c_str(), tipValue);
			}
		}
	}
}

void NetworkSelectDialog::drawColumnPlayers(unsigned int id, int row, int col, 
	float x, float y, float w)
{
	int gamesrow = gamesIconTable_->getSelected();
	if (gamesrow < 0 || gamesrow >= ServerBrowser::instance()->getServerList().getNoEntries())
	{
		return;
	}

	std::string valuestr = 
		ServerBrowser::instance()->getServerList().getEntryValue(
			gamesrow, S3D::formatStringBuffer("%s%i", playerscols[col].dataName, row));
	const char *value = valuestr.c_str();

	Vector color(0.3f, 0.3f, 0.3f);
	GLWFont::instance()->getGameFont()->drawWidth(w, 
		color, 
		10.0f, x + 3.0f, y + 5.0f, 0.0f, value);
}

bool NetworkSelectDialog::serverCompatable(std::string pversion, std::string version)
{
	if (pversion.size() > 0 && 
		0 != strcmp(pversion.c_str(), S3D::ScorchedProtocolVersion.c_str()))
	{
		return false;
	}

	return true;
}

void NetworkSelectDialog::rowSelected(unsigned int id, int row)
{
	if (id == gamesIconTable_->getId()) rowSelectedGames(id, row);
}

void NetworkSelectDialog::rowSelectedGames(unsigned int id, int row)
{
	if (row < 0 || row >= ServerBrowser::instance()->getServerList().getNoEntries())
	{
		return;
	}

	// Check if we have any information for this row
	std::string ipaddress = ServerBrowser::instance()->
		getServerList().getEntryValue(row, "address");
	if (ipaddress.empty()) return;

	// Check if this server is compatible
	std::string protocolVersion = 
		ServerBrowser::instance()->getServerList().getEntryValue(row, "protocolversion");
	std::string version =
		ServerBrowser::instance()->getServerList().getEntryValue(row, "version");
	if (!serverCompatable(protocolVersion, version))
	{
		MsgBoxDialog::instance()->show(
			S3D::formatStringBuffer(
			"Warning: This server is running a incompatable version of Scorched3D.\n"
			"You cannot connect to this server.\n\n"
			"This server is running Scorched build %s (%s).\n"
			"You are running Scorched build %s (%s).\n\n"					
			"The latest version of Scorched3D can be downloaded from \n"
			"http://www.scorched3d.co.uk\n",
			version.c_str(), protocolVersion.c_str(),
			S3D::ScorchedVersion.c_str(), S3D::ScorchedProtocolVersion.c_str()));
	}

	// Set ip address for this server
	ipaddress_->setText(ipaddress.c_str());

	// Set players for this server
	std::string players = 
		ServerBrowser::instance()->getServerList().getEntryValue(row, "noplayers");
	int noplayers = atoi(players.c_str());
	playersIconTable_->setItemCount(noplayers);
}

void NetworkSelectDialog::rowChosen(unsigned int id, int row)
{
	if (id == gamesIconTable_->getId()) rowChosenGames(id, row);
}

void NetworkSelectDialog::rowChosenGames(unsigned int id, int row)
{
	if (ok_->getEnabled()) buttonDown(ok_->getId());
}

void NetworkSelectDialog::columnSelected(unsigned int id, int col)
{
	if (id == gamesIconTable_->getId()) columnSelectedGames(id, col);
}

void NetworkSelectDialog::columnSelectedGames(unsigned int id, int col)
{
	ServerBrowser::instance()->getServerList().sortEntries(gamescols[col].dataName);
}

void NetworkSelectDialog::display()
{
	ipaddress_->setText("");
	refreshType_->setCurrentText("Internet");
}

void NetworkSelectDialog::hide()
{
	stopRefresh();
}

void NetworkSelectDialog::updateTable()
{
	// Check if we are refreshing
	// Set the button accordingly
	if (!ServerBrowser::instance()->getRefreshing())
	{
		refresh_->setText(LANG_RESOURCE("Refresh List"));
	}
	else
	{
		refresh_->setText(LANG_RESOURCE("Stop Refresh"));
	}

	// Check if we have more items to display
	if (invalidateId_ != ServerBrowser::instance()->
		getServerList().getRefreshId())
	{
		invalidateId_ = ServerBrowser::instance()->
			getServerList().getRefreshId();
		gamesIconTable_->setItemCount(
			ServerBrowser::instance()->getServerList().getNoEntries());
	}
}

void NetworkSelectDialog::select(unsigned int id, const int pos, GLWSelectorEntry value)
{
	if (0 == strcmp(refreshType_->getCurrentText(), "Favourites"))
	{
		favourites_->setText(LANG_RESOURCE("Del Favourite"));
	}
	else
	{
		favourites_->setText(LANG_RESOURCE("Add Favourite"));
	}

	startRefresh();
}

void NetworkSelectDialog::startRefresh()
{
	stopRefresh();

	gamesIconTable_->setItemCount(0);
	playersIconTable_->setItemCount(0);

	ServerBrowser::RefreshType t = ServerBrowser::RefreshNone;
	if (0 == strcmp(refreshType_->getCurrentText(), "LAN")) t = ServerBrowser::RefreshLan;
	else if (0 == strcmp(refreshType_->getCurrentText(), "Internet")) t = ServerBrowser::RefreshNet;
	else if (0 == strcmp(refreshType_->getCurrentText(), "Favourites")) t = ServerBrowser::RefreshFavourites;

	ServerBrowser::instance()->refreshList(t);
	updateTable();
}

void NetworkSelectDialog::stopRefresh()
{
	ServerBrowser::instance()->cancel();
	updateTable();
}

void NetworkSelectDialog::textChanged(unsigned int id, const char *text)
{
	ok_->setEnabled(text[0]!='\0');
	favourites_->setEnabled(text[0]!='\0');
}

void NetworkSelectDialog::buttonDown(unsigned int id)
{
	if (id == refresh_->getId())
	{
		if (ServerBrowser::instance()->getRefreshing())
		{
			stopRefresh();
		}
		else
		{
			startRefresh();
		}
	}
	else if (id == ok_->getId())
	{
		GLWWindowManager::instance()->hideWindow(id_);

		if (ipaddress_->getText()[0])
		{
			ClientParams::instance()->reset();
			ClientParams::instance()->setConnect(ipaddress_->getText().c_str());
			ClientMain::startClient();
		}
	}
	else if (id == favourites_->getId())
	{
		if (!ipaddress_->getText().empty())
		{
			std::set<std::string> favs = 
				ServerBrowser::instance()->getCollect().getFavourites();

			if (0 == strcmp(refreshType_->getCurrentText(), "Favourites"))
			{
				favs.erase(ipaddress_->getText().c_str());
			}
			else 
			{
				favs.insert(ipaddress_->getText().c_str());
			}
			ServerBrowser::instance()->getCollect().setFavourites(favs);

			if (0 == strcmp(refreshType_->getCurrentText(), "Favourites"))
			{
				startRefresh();
			}
		}
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}
