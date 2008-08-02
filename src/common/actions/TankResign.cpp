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

#include <actions/TankResign.h>
#include <engine/ScorchedContext.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankTeamScore.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <common/OptionsScorched.h>

TankResign::TankResign(unsigned int playerId) :
	ActionReferenced("TankResign"),
	firstTime_(true),
	playerId_(playerId)
{

}

TankResign::~TankResign()
{
}

void TankResign::init()
{
}

void TankResign::simulate(fixed frameTime, bool &remove)
{
	if (firstTime_)
	{
		firstTime_ = false;
		Tank *tank = 
			context_->getTankContainer().getTankById(playerId_);
		if (tank && tank->getState().getState() == TankState::sNormal)
		{
			// update player assists when this player resigns
			int moneyPerAssist = 
				context_->getOptionsGame().getMoneyWonPerAssistPoint() *
					5;
			int scorePerAssist = context_->getOptionsGame().getScorePerAssist();

			// Update assists
			std::set<unsigned int> &hurtBy = 
				tank->getScore().getHurtBy();
			std::set<unsigned int>::iterator itor;
			for (itor = hurtBy.begin();
				itor != hurtBy.end();
				itor++)
			{
				unsigned int hurtByPlayer = (*itor);
				Tank *hurtByTank = 
					context_->getTankContainer().getTankById(hurtByPlayer);
				if (!hurtByTank) continue;

				// Only score when the tank does not hurt itself
				if (hurtByTank == tank) continue;

				// or a team member
				if ((context_->getOptionsGame().getTeams() > 1) &&
					(hurtByTank->getTeam() == tank->getTeam())) continue;

				// Update assist score
				hurtByTank->getScore().setAssists(
					hurtByTank->getScore().getAssists() + 1);
				hurtByTank->getScore().setMoney(
					hurtByTank->getScore().getMoney() + moneyPerAssist);
				hurtByTank->getScore().setScore(
					hurtByTank->getScore().getScore() + scorePerAssist);

				if (hurtByTank->getTeam() > 0)
				{
					context_->getTankTeamScore().addScore(
						scorePerAssist, hurtByTank->getTeam());
				}
			}

			// tank is dead
			tank->getState().setState(TankState::sDead);

			// This tank has lost a life
			if (tank->getState().getMaxLives() > 0)
			{
				tank->getState().setLives(
					tank->getState().getLives() - 1);
			}

#ifndef S3D_SERVER
			{
				ChannelText text("combat",
					S3D::formatStringBuffer("[p:%s] resigned from round", tank->getName()));
				//info.setPlayerId(playerId_);
				ChannelManager::showText(*context_, text);
			}
#endif // #ifndef S3D_SERVER
		}
	}

	remove = true;
	Action::simulate(frameTime, remove);
}

std::string TankResign::getActionDetails()
{
	return S3D::formatStringBuffer("%u", playerId_);
}
