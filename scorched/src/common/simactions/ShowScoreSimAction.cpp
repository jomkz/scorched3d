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

#include <simactions/ShowScoreSimAction.h>
#include <engine/ActionController.h>
#include <actions/ShowScoreAction.h>
#include <common/OptionsScorched.h>
#include <common/StatsLogger.h>
#include <common/ChannelManager.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankTeamScore.h>
#include <tank/TankContainer.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankSort.h>

REGISTER_CLASS_SOURCE(ShowScoreSimAction);

ShowScoreSimAction::ShowScoreSimAction() :
	scoreTime_(0), finalScore_(false)
{
}

ShowScoreSimAction::ShowScoreSimAction(fixed scoreTime, bool finalScore) :
	scoreTime_(scoreTime), finalScore_(finalScore)
{
}

ShowScoreSimAction::~ShowScoreSimAction()
{
}

bool ShowScoreSimAction::invokeAction(ScorchedContext &context)
{
	scoreWinners(context);

	context.getActionController().addAction(
		new ShowScoreAction(scoreTime_, finalScore_));
	return true;
}

bool ShowScoreSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(scoreTime_);
	buffer.addToBuffer(finalScore_);
	return true;
}

bool ShowScoreSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(scoreTime_)) return false;
	if (!reader.getFromBuffer(finalScore_)) return false;
	return true;
}

void ShowScoreSimAction::scoreWinners(ScorchedContext &context)
{
	// Calculate all the tanks interest
	float interest = float(context.
		getOptionsGame().getInterest()) / 100.0f;
	std::map<unsigned int, Tank *> &playingTank = 
		context.getTankContainer().getPlayingTanks();

	int moneyWonForRound = 
		context.getOptionsGame().getMoneyWonForRound();
	int moneyWonForLives =
	        context.getOptionsGame().getMoneyWonForLives();
	int scoreWonForRound = 
		context.getOptionsGame().getScoreWonForRound();
	int scoreWonForLives = 
		context.getOptionsGame().getScoreWonForLives();
	int skillWonForRound = 
		context.getOptionsGame().getSkillForRound();

	// Add score for each life left
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = playingTank.begin();
		itor != playingTank.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		tank->getState().setMoveId(0);

		if (!tank->getState().getTankPlaying()) continue;
		if (!tank->getAlive()) continue;

		tank->getScore().setScore(tank->getScore().getScore() + 
			(scoreWonForLives * tank->getState().getLives()));
	}
				
	// Is it a team game?
	if (context.getOptionsGame().getTeams() > 1)
	{
		// Yes, check which team has won and give points accordingly
		// Add score for round wins
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = playingTank.begin();
			itor != playingTank.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			if (!tank->getState().getTankPlaying()) continue;
			if (!tank->getAlive()) continue;

			context.getTankTeamScore().addScore(
				scoreWonForLives * tank->getState().getLives(), tank->getTeam());
		}

		std::set<int> winningTeams;
		int winningTeam = 
			context.getTankTeamScore().getWonGame();
		if (winningTeam != 0)
		{
			winningTeams.insert(winningTeam);
			context.getTankTeamScore().addScore(
				scoreWonForRound, winningTeam);
		}
		else
		{
			for (itor = playingTank.begin();
				itor != playingTank.end();
				itor++)
			{
				Tank *tank = (*itor).second;
				if (!tank->getState().getTankPlaying()) continue;
				if (!tank->getAlive()) continue;
				if (winningTeams.find(tank->getTeam()) != winningTeams.end()) continue;

				context.getTankTeamScore().addScore(
					scoreWonForRound, tank->getTeam());
				winningTeams.insert(tank->getTeam());
			}
		}

		for (itor = playingTank.begin();
			itor != playingTank.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			if (!tank->getState().getTankPlaying()) continue;
			if (winningTeams.find(tank->getTeam()) == winningTeams.end()) continue;

			StatsLogger::instance()->tankWon(tank);
			tank->getScore().setMoney(
				tank->getScore().getMoney() + moneyWonForRound);
			tank->getScore().setMoney(
				tank->getScore().getMoney() + (moneyWonForLives * tank->getState().getLives()));
			tank->getScore().setWins(
				tank->getScore().getWins() + 1);
			tank->getScore().setSkill(
				tank->getScore().getSkill() + skillWonForRound);
		}
	}
	else
	{
		// Check which player has won and give points accordingly
		bool tankWon = false;
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = playingTank.begin();
			itor != playingTank.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			if (!tank->getState().getTankPlaying()) continue;
			if (!tank->getScore().getWonGame()) continue;

			StatsLogger::instance()->tankWon(tank);
			tank->getScore().setMoney(
				tank->getScore().getMoney() + moneyWonForRound);
			tank->getScore().setMoney(
				tank->getScore().getMoney() + (moneyWonForLives * tank->getState().getLives()));
			tank->getScore().setWins(
				tank->getScore().getWins() + 1);
			tank->getScore().setScore(
				tank->getScore().getScore() + scoreWonForRound);
			tank->getScore().setSkill(
				tank->getScore().getSkill() + skillWonForRound);

			tankWon = true;
		}

		if (!tankWon)
		{
			for (itor = playingTank.begin();
				itor != playingTank.end();
				itor++)
			{
				Tank *tank = (*itor).second;
				if (!tank->getState().getTankPlaying()) continue;
				if (!tank->getAlive()) continue;

				StatsLogger::instance()->tankWon(tank);
				tank->getScore().setMoney(
					tank->getScore().getMoney() + moneyWonForRound);
				tank->getScore().setMoney(
					tank->getScore().getMoney() + (moneyWonForLives * tank->getState().getLives()));
				tank->getScore().setWins(
					tank->getScore().getWins() + 1);	
				tank->getScore().setScore(
					tank->getScore().getScore() + scoreWonForRound);
				tank->getScore().setSkill(
					tank->getScore().getSkill() + skillWonForRound);
			}
		}
	}

	// Give interest and round played money to all tanks
	{
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = playingTank.begin();
			itor != playingTank.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			if (!tank->getState().getTankPlaying()) continue;

			int addMoney = int(float(tank->getScore().getMoney()) * interest) +
				context.getOptionsGame().getMoneyPerRound();
			tank->getScore().setMoney(tank->getScore().getMoney() + addMoney);

			// Make sure this tank is dead
			if (tank->getState().getState() == TankState::sNormal)
			{
				tank->getState().setState(TankState::sDead);
			}
		}
	}

	// Its the very last round, score the overall winner
	if (finalScore_)
	{
		scoreOverallWinner(context);
	}

	// Update the stats for the players before sending out the
	// stats message
	std::map<unsigned int, Tank *> &tanks = 
		context.getTankContainer().getPlayingTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;

		// Money earned
		int newMoney = tank->getScore().getTotalMoneyEarnedStat();
		int scoreAdded = (newMoney * 
			context.getOptionsGame().getScorePerMoney())
			/ 1000;
		tank->getScore().setScore(tank->getScore().getScore() + scoreAdded);

		// Ensure stats are uptodate
		StatsLogger::instance()->updateStats(tank);

		// Reset the totaled stats
		tank->getScore().resetTotalEarnedStats();

		if (context.getServerMode())
		{
			// Get the new tanks rank
			StatsLogger::TankRank rank = StatsLogger::instance()->tankRank(tank);
			tank->getScore().setRank(rank.rank);
		}
	}

	StatsLogger::instance()->periodicUpdate();
}

void ShowScoreSimAction::scoreOverallWinner(ScorchedContext &context)
{
	int skillWonForMatch = 
		context.getOptionsGame().getSkillForMatch();

	if (context.getOptionsGame().getTeams() == 1)
	{
		std::list<Tank *> sortedTanks;
		std::map<unsigned int, Tank *> &tanks = 
			context.getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *current = (*itor).second;
			if (current->getState().getTankPlaying())
			{
				sortedTanks.push_back(current);
			}
		}
		
		TankSort::getSortedTanks(sortedTanks, context);
		if (!sortedTanks.empty())
		{
			Tank *topScore = *(sortedTanks.begin());

			LangString names;
			std::list<Tank *> winners;
            std::list<Tank *>::iterator scoreitor;
			for (scoreitor = sortedTanks.begin();
				scoreitor != sortedTanks.end();
				scoreitor++)
			{
				Tank *current = *scoreitor;
				if (topScore->getScore().getScore() == 
					current->getScore().getScore())
				{
					winners.push_back(current);
					if (!names.empty()) names.append(LANG_STRING(","));
					names.append(current->getTargetName());

					// Score the winning tank as the overall winner
					StatsLogger::instance()->tankOverallWinner(current);

					current->getScore().setSkill(
						current->getScore().getSkill() + skillWonForMatch);
				}
			}

			if (winners.size() == 1)
			{
				ChannelText text("banner",
					"PLAYER_OVERALL_WINNER",
					"{0} is the overall winner!", 
					names);
				ChannelManager::showText(context, text);
			}
			else
			{
				ChannelText text("banner",
					"PLAYERS_OVERALL_WINNERS",
					"{0} are the overall winners!", 
					names);
				ChannelManager::showText(context, text);
			}
		}
	}
	else
	{
		int winningTeam = TankSort::getWinningTeam(context);
		if (winningTeam == 0)
		{
			ChannelText text("banner", 
				"GAME_DRAWN",
				"The game is a draw!");
			ChannelManager::showText(context, text);
		}
		else
		{
			ChannelText text("banner",
				"TEAM_OVERALL_WINNER",
				"{0} team is the overall winner!",
				TankColorGenerator::getTeamName(winningTeam));
			ChannelManager::showText(context, text);
		}
		
		// Score all the winning tanks as overall winners
		if (winningTeam != 0)
		{
			std::map<unsigned int, Tank *> &tanks = 
				context.getTankContainer().getPlayingTanks();
			std::map<unsigned int, Tank *>::iterator itor;
			for (itor = tanks.begin();
				itor != tanks.end();
				itor++)
			{
				Tank *tank = (*itor).second;
				if (tank->getTeam() == winningTeam)
				{
					if (tank->getState().getTankPlaying())
					{
						StatsLogger::instance()->tankOverallWinner(tank);
						tank->getScore().setSkill(
							tank->getScore().getSkill() + skillWonForMatch);
					}
				}
			}
		}
	}
}
