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

#include <simactions/TankTeamBallanceSimAction.h>
#include <tank/TankContainer.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankModelStore.h>
#include <tank/TankModelContainer.h>
#include <common/OptionsScorched.h>
#include <common/ChannelManager.h>
#include <algorithm>

static inline bool lt_score(const Tank *o1, const Tank *o2) 
{ 
	return ((Tank*)o1)->getScore().getScore() > ((Tank *)o2)->getScore().getScore();
}

static inline bool lt_bots(const Tank *o1, const Tank *o2) 
{ 
	return ((Tank*)o1)->getTankAI() < ((Tank *)o2)->getTankAI();
}

REGISTER_CLASS_SOURCE(TankTeamBallanceSimAction);

TankTeamBallanceSimAction::TankTeamBallanceSimAction()
{
}

TankTeamBallanceSimAction::~TankTeamBallanceSimAction()
{
}

bool TankTeamBallanceSimAction::invokeAction(ScorchedContext &context)
{
	// Tell people whats going on
	ChannelText text("info",
		"AUTO_BALLANCE_TEAMS",
		"Auto ballancing teams");
	ChannelManager::showText(context, text);

	// Make sure everyone is in a team if they should be
	std::map<unsigned int, Tank *> &playingTanks = 
		context.getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (context.getOptionsGame().getTeams() > 1 &&
				current->getTeam() == 0) current->setTeam(1); 
			if (context.getOptionsGame().getTeams() == 1 &&
				current->getTeam() > 0) current->setTeam(0); 
		}
	}

	// Do we check teams ballance
	if (context.getOptionsGame().getTeams() != 1)
	{
		// Check for team ballance types
		switch (context.getOptionsGame().getTeamBallance())
		{
			case OptionsGame::TeamBallanceAuto:
			case OptionsGame::TeamBallanceAutoByScore:
			case OptionsGame::TeamBallanceAutoByBots:
				checkTeamsAuto(context);
				break;
			case OptionsGame::TeamBallanceBotsVs:
				checkTeamsBotsVs(context);
				break;
			default:
				break;
		}
	}

	// Make sure everyone is using a team model
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			TankModel *model = 
				context.getTankModels().getModelByName(
					current->getModelContainer().getTankModelName(),
					current->getTeam(),
					current->isTemp());
			if (0 != strcmp(model->getName(),
				current->getModelContainer().getTankModelName()))
			{
				// The model is not allowed for this team,
				// use the correct model
				current->getModelContainer().setTankModelName(
					model->getName(), 
					model->getName(),
					model->getTypeName());
			}
		}
	}

	return true;
}

bool TankTeamBallanceSimAction::writeMessage(NetBuffer &buffer)
{
	return true;
}
bool TankTeamBallanceSimAction::readMessage(NetBufferReader &reader)
{
	return true;
}

void TankTeamBallanceSimAction::minMaxTeams(ScorchedContext &context,
	std::vector<Tank *> *teamPlayers,
	std::vector<Tank *> *&minPlayers,
	std::vector<Tank *> *&maxPlayers)
{
	// Clear all team lists
	for (int i=0; i<context.getOptionsGame().getTeams(); i++)
	{
		teamPlayers[i].clear();
	}

	// Count players in each team
	std::map<unsigned int, Tank *> &playingTanks = 
		context.getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (current->getTeam() > 0)
			{
				teamPlayers[current->getTeam() - 1].push_back(current);
			}
		}
	}

	// Find the teams with the min and max players in them
	minPlayers = &teamPlayers[0];
	maxPlayers = &teamPlayers[0];
	for (int i=0; i<context.getOptionsGame().getTeams(); i++)
	{
		if (teamPlayers[i].size() < minPlayers->size()) minPlayers = &teamPlayers[i];
		if (teamPlayers[i].size() > maxPlayers->size()) maxPlayers = &teamPlayers[i];			
	}
}

void TankTeamBallanceSimAction::checkTeamsAuto(ScorchedContext &context)
{
	// Find the teams with the min and max players in them
	std::vector<Tank *> teamPlayers[4], *minPlayers = 0, *maxPlayers = 0;

	// Is the difference between the min and max teams >= 2 players
	for (minMaxTeams(context, teamPlayers, minPlayers, maxPlayers);
		maxPlayers->size() - minPlayers->size() >= 2;
		minMaxTeams(context, teamPlayers, minPlayers, maxPlayers))
	{
		// Sort teams (if needed)
		if (context.getOptionsGame().getTeamBallance() ==
			OptionsGame::TeamBallanceAutoByScore)
		{
			std::sort(minPlayers->begin(), minPlayers->end(), lt_score); 
			std::sort(maxPlayers->begin(), maxPlayers->end(), lt_score); 
		}
		else if (context.getOptionsGame().getTeamBallance() ==
			OptionsGame::TeamBallanceAutoByBots)
		{
			std::sort(minPlayers->begin(), minPlayers->end(), lt_bots); 
			std::sort(maxPlayers->begin(), maxPlayers->end(), lt_bots); 
		}
	
		// Find out which team has the least players
		for (int i=0; i<context.getOptionsGame().getTeams(); i++)
		{	
			if (minPlayers == &teamPlayers[i])
			{
				// Ballance the teams
				Tank *tank = maxPlayers->back();
				tank->setTeam(i+1);
			}
		}
	}
}

void TankTeamBallanceSimAction::checkTeamsBotsVs(ScorchedContext &context)
{
	std::map<unsigned int, Tank *> &playingTanks = 
		context.getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (current->getDestinationId() == 0) current->setTeam(1);
			else current->setTeam(2);
		}
	}
}
