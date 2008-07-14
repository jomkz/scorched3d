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

#include <dialogs/ScoreDialog.h>
#include <tank/TankSort.h>
#include <tank/TankContainer.h>
#include <tank/TankTeamScore.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankAvatar.h>
#include <tankai/TankAI.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWColors.h>
#include <client/ClientParams.h>
#include <graph/OptionsDisplay.h>
#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <engine/GameState.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <client/ClientScoreHandler.h>
#include <server/ScorchedServer.h>
#include <stdio.h>

static const float rankLeft = 15.0f;
static const float iconLeft = 5.0f;
static const float nameLeft = 35.0f;
static const float livesLeft = 230.0f;
static const float killsLeft = 255.0f;
static const float assistsLeft = 280.0f;
static const float winsLeft = 305.0f;
static const float moneyLeft = 325.0f;
static const float scoreLeft = 405.0f;
static const float statsLeft = 475.0f;
static const float readyLeft = 515.0f;
static const float lineSpacer = 10.0f;

ScoreDialog *ScoreDialog::instance_ = 0;
ScoreDialog *ScoreDialog::instance2_ = 0;

ScoreDialog *ScoreDialog::instance()
{
	if (!instance_)
	{
		instance_ = new ScoreDialog;
	}

	return instance_;
}

ScoreDialog *ScoreDialog::instance2()
{
	if (!instance2_)
	{
		instance2_ = new ScoreDialog;
	}

	return instance2_;
}

ScoreDialog::ScoreDialog() :
	GLWWindow("Score", 10.0f, 10.0f, 525.0f, 310.0f, eTransparent |eSmallTitle,
		"Shows the current score for all players."),
	lastScoreValue_(0), lastMoneyValue_(0), lastNoPlayers_(0)
{

}

ScoreDialog::~ScoreDialog()
{

}

void ScoreDialog::display()
{
	GLWWindow::display();
	calculateScores();

	if (OptionsDisplay::instance()->getHideFinalScore() &&
		ScorchedClient::instance()->getGameState().getState() ==
		ClientState::StateScore)
	{
		GLWWindowManager::instance()->hideWindow(getId());
	}
}

void ScoreDialog::calculateScores()
{
	lastScoreValue_ = lastMoneyValue_ = 0;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		lastMoneyValue_ += tank->getScore().getMoney();
		lastScoreValue_ += tank->getScore().getScore();
	}

	lastNoPlayers_ = tanks.size();
	sortedTanks_.clear();
	TankSort::getSortedTanksIds(
		ScorchedClient::instance()->getContext(), sortedTanks_);
}

void ScoreDialog::windowInit(const unsigned state)
{
	needCentered_ = true;
}

void ScoreDialog::draw()
{
	float h = sortedTanks_.size() * lineSpacer + 80.0f;
	if (ScorchedClient::instance()->getOptionsGame().getTeams() > 1)
	{
		h += (lineSpacer * 2.0f) * (ScorchedClient::instance()->getOptionsGame().getTeams() - 1) +
			lineSpacer * 2.0f;
	}
	setH(h);

	static size_t noTanks = 0;
	if (noTanks != sortedTanks_.size())
	{
		needCentered_ = true;
		noTanks = sortedTanks_.size();
	}

	GLWWindow::draw();
	GLState newState(GLState::TEXTURE_OFF | GLState::DEPTH_OFF);

	bool finished = false;
	if (ScorchedClient::instance()->getGameState().getState() == 
		ClientState::StateScore)
	{
		finished = ClientScoreHandler::instance()->getFinalScore();
	}

	Vector white(0.9f, 0.9f, 1.0f);
	bool server = (ClientParams::instance()->getConnectedToServer());
	{
		const char *text = "Current Rankings";
		if (finished)
		{
			text = "Final Rankings";
		}
		else if (ScorchedClient::instance()->getGameState().getState() == 
			ClientState::StateGetPlayers)
		{
			finished = true;
			if (ScorchedClient::instance()->getTankContainer().getNoOfNonSpectatorTanks() <
				ScorchedClient::instance()->getOptionsGame().getNoMinPlayers())
			{
				text = "Waiting for more players";
			}
			else
			{
				text = "Waiting to join game";
			}
		}
		else if (server)
		{
			text = ScorchedClient::instance()->getOptionsGame().getServerName();
		}

		GLWFont::instance()->getGameShadowFont()->draw(
			GLWColors::black,
				20,
				x_ + 8.0f - 2.0f, y_ + h_ - 22.0f + 2.0f, 0.0f,
				text);
		GLWFont::instance()->getGameFont()->draw(
				white,
				20,
				x_ + 8.0f, y_ + h_ - 22.0f, 0.0f,
				text);
	}

	if (!finished) 
	{
		char moves[256];
		moves[0] = '\0';
		if (ScorchedClient::instance()->getOptionsGame().getNoMaxRoundTurns() > 0)
		{
			snprintf(moves, 256, ",Move %i of %i", 
				ScorchedClient::instance()->getOptionsTransient().getCurrentGameNo(),
				ScorchedClient::instance()->getOptionsGame().getNoMaxRoundTurns());
		}
		std::string rounds = S3D::formatStringBuffer("Round %i of %i%s",
				ScorchedClient::instance()->getOptionsTransient().getCurrentRoundNo(),
				ScorchedClient::instance()->getOptionsGame().getNoRounds(),
				moves);

		float roundsWidth = GLWFont::instance()->getGameFont()->getWidth(
			10, rounds);
		GLWFont::instance()->getGameFont()->draw(
			white,
			10,
			x_ + 470 - roundsWidth, y_ + h_ - 40.0f, 0.0f,
			rounds);
	}

	float y = lineSpacer + 10.0f;
	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + nameLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"Name");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, "Name", "The name of the player",
		x_ + nameLeft, y_ + h_ - y - lineSpacer - 26.0f, 100.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + killsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"K");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, "Kills", 
		S3D::formatStringBuffer("The number of players this player has killed.\n%i score awarded per kill.",
		ScorchedClient::instance()->getOptionsGame().getScorePerKill()),
		x_ + killsLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + winsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"W");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, "Wins", 
		S3D::formatStringBuffer("The number of rounds this player has won.\n%i score awarded per round won.",
		ScorchedClient::instance()->getOptionsGame().getScoreWonForRound()),
		x_ + winsLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + assistsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"A");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, "Assists", 
		S3D::formatStringBuffer("The number of kills this player has assisted in.\n%i score awarded per assist.",
		ScorchedClient::instance()->getOptionsGame().getScorePerAssist()),
		x_ + assistsLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + moneyLeft + 10, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"$");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, "Money", 
		S3D::formatStringBuffer("The amount of money this player has.\n%.1f score awarded per dollar.",
		float(ScorchedClient::instance()->getOptionsGame().getScorePerMoney()) / 1000.0f),
		x_ + moneyLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + scoreLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"Score");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, "Score", 
		"The current score for this player.\nCalculated from the number of kills, wins, money and bonus score awards.",
		x_ + scoreLeft, y_ + h_ - y - lineSpacer - 26.0f, 80.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + statsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"Rank");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, "Rank", "The current online ranking for this player.",
		x_ + statsLeft, y_ + h_ - y - lineSpacer - 26.0f, 40.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
		white,
		12,
		x_ + livesLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
		"L");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, "Lives", 
		S3D::formatStringBuffer("The current number of lives this player has left.\n%i score awarded for each life remaining.",
		ScorchedClient::instance()->getOptionsGame().getScoreWonForLives()),
		x_ + livesLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);
	y+= lineSpacer + lineSpacer;

	int tmpLastScoreValue = 0;
	int tmpLastMoneyValue = 0;
	if (ScorchedClient::instance()->getOptionsGame().getTeams() > 1)
	{
		int winningTeam = TankSort::getWinningTeam(
			ScorchedClient::instance()->getContext());

		for (int i=0; i<ScorchedClient::instance()->getOptionsGame().getTeams(); i++)
		{
			bool someTeam = false;
			std::list<unsigned int>::iterator itor;
			for (itor = sortedTanks_.begin();
				itor != sortedTanks_.end();
				itor ++)
			{
				unsigned int playerId = (*itor);
				Tank *current = ScorchedClient::instance()->getTankContainer().getTankById(playerId);
				if (current && current->getTeam() == (i + 1) && !current->getState().getSpectator()) 
				{
					someTeam = true;
					addLine(current, y, (char *)((winningTeam==(i+1))?"1":"2"), finished);
					
					tmpLastScoreValue += current->getScore().getScore();
					tmpLastMoneyValue += current->getScore().getMoney();
					y+= lineSpacer;
				}
			}
			if (someTeam)
			{
				addScoreLine(y, 
					TankColorGenerator::getTeamColor(i + 1),
					ScorchedClient::instance()->getContext().getTankTeamScore().getScore(i+1));
				y+= lineSpacer;
				y+= lineSpacer;
			}
		}
	}
	else
	{
		// No Team
		int rank = 1;
		char strrank[10];
		std::list<unsigned int>::iterator itor;
		for (itor = sortedTanks_.begin();
			itor != sortedTanks_.end();
			itor ++, rank++)
		{
			unsigned int playerId = (*itor);
			Tank *current = ScorchedClient::instance()->getTankContainer().getTankById(playerId);
			if (current && !current->getState().getSpectator()) 
			{
				snprintf(strrank, 10, "%i", rank);

				addLine(current, y, strrank, finished);
				tmpLastScoreValue += current->getScore().getScore();
				tmpLastMoneyValue += current->getScore().getMoney();
				y+= lineSpacer;
			}
		}	
	}
	y+= lineSpacer / 1.5f;
	// Spectators
	std::list<unsigned int>::iterator itor;
	for (itor = sortedTanks_.begin();
		itor != sortedTanks_.end();
		itor ++)
	{
		unsigned int playerId = (*itor);
		Tank *current = ScorchedClient::instance()->
			getTankContainer().getTankById(playerId);
		if (current && current->getState().getSpectator()) 
		{
			addLine(current, y, " ", false);
			y+= lineSpacer;
		}
	}	

	std::map<unsigned int, Tank *> &realTanks = 
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	if (tmpLastScoreValue != lastScoreValue_ ||
		tmpLastMoneyValue != lastMoneyValue_ ||
		realTanks.size() != lastNoPlayers_)
	{
		calculateScores();
	}
}

void ScoreDialog::addScoreLine(float y, Vector &color, int score)
{
	float textX = x_;
	float textY  = y_ + h_ - y - lineSpacer - 30.0f;

	// Print the name on the screen
	GLWFont::instance()->getGameFont()->draw(
		color,
		10,
		textX + scoreLeft, textY, 0.0f,
		S3D::formatStringBuffer("%i", score));
}

void ScoreDialog::addLine(Tank *current, float y, char *rank, bool finished)
{
	float textX = x_;
	float textY  = y_ + h_ - y - lineSpacer - 25.0f;
	bool currentPlayer = false;
	
	// Print a highlight behind the current clients player
	if (!current->getState().getSpectator() &&
		current->getDestinationId() == 
		ScorchedClient::instance()->getTankContainer().getCurrentDestinationId())
	{
		GLState state(GLState::BLEND_ON); 

		glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
		glBegin(GL_QUADS);
			glVertex2f(x_ + w_ - 3.0f, textY + lineSpacer - 1.0f);
			glVertex2f(x_ + 3.0f, textY + lineSpacer - 1.0f);
			glVertex2f(x_ + 3.0f, textY - 1.0f);
			glVertex2f(x_ + w_ - 3.0f, textY - 1.0f);
		glEnd();
	}

	// Form the name
	static char name[256];
	// Max strcat usage 12
	snprintf(name, sizeof(name) - 12, "%s", current->getName());
	if (finished && ! ClientParams::instance()->getConnectedToServer())
	{
		strcat(name, " (");
		Tank *serverTank = 
			ScorchedServer::instance()->getTankContainer().getTankById(
			current->getPlayerId());
		TankAI *tankAI = serverTank->getTankAI();
		if (tankAI) strcat(name, tankAI->getName());
		else strcat(name, "Human");
		strcat(name, ")");
	}
	else if (current->getState().getState() != TankState::sNormal)
	{
		strcat(name, " (");
		strcat(name, current->getState().getSmallStateString());
		strcat(name, ")");
	}

	if (current->getState().getSpectator())
	{
		name[50] = '\0'; // Limit length

		// Print the name on the screen
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + nameLeft, textY, 0.0f,
			name);
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + readyLeft, textY, 0.0f,
			S3D::formatStringBuffer("%2s",
			((current->getState().getReadyState() == TankState::SNotReady)?"*":" ")));
	}
	else
	{
		name[26] = '\0'; // Limit length

		GLState state(GLState::TEXTURE_ON);
		glColor3f(1.0f, 1.0f, 1.0f);
		current->getAvatar().getTexture()->draw();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(textX + iconLeft,
				textY + 2.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(textX + iconLeft + 8.0f, 
				textY + 2.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(textX + iconLeft + 8.0f,
				textY + 10.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(textX + iconLeft,
				textY + 10.0f);
        glEnd();

		// Print the name on the screen
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + rankLeft, textY, 0.0f,
			rank);
		GLWFont::instance()->getGameFont()->drawWidth(
			killsLeft - nameLeft,
			current->getColor(),
			10,
			textX + nameLeft, textY, 0.0f,
			name);
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + killsLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", current->getScore().getKills()));
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + moneyLeft, textY, 0.0f,
			S3D::formatStringBuffer("$%i", current->getScore().getMoney()));
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + winsLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", current->getScore().getWins()));
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + scoreLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", current->getScore().getScore()));
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + assistsLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", current->getScore().getAssists()));
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + readyLeft, textY, 0.0f,
			((current->getState().getReadyState() == TankState::SNotReady)?"*":" "));
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + statsLeft, textY, 0.0f,
			current->getScore().getStatsRank());
		GLWFont::instance()->getGameFont()->draw(
			current->getColor(),
			10,
			textX + livesLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", current->getState().getLives()));
	}
}
