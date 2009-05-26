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

#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <time.h>
#include <math.h>

OptionsTransient::OptionsTransient(OptionsScorched &optionsGame) :
	optionsGame_(optionsGame), newGame_(false),
	currentRoundNo_(options_, "CurrentRoundNo", 
		"The current number of rounds played in this game", 0, 0),
	currentGameNo_(options_, "CurrentGameNo",
		"The current game", 0, 0),
	windAngle_(options_, "WindAngle",
		"The current wind angle (direction)", 0, 0),
	windStartAngle_(options_, "WindStartAngle",
		"The angle (direction) the wind started the round on", 0, 0),
	windSpeed_(options_, "WindSpeed",
		"The current speed of the wind", 0, 0),	
	windDirection_(options_, "WindDirection",
		"The current wind direction vector", 0, FixedVector::getNullVector()),
	wallType_(options_, "WallType",
		"The current wall type", 0, 0)
{
	
}

OptionsTransient::~OptionsTransient()
{
}

const char *OptionsTransient::getGameType()
{
	const char *gameType = "Unknown";
	switch (optionsGame_.getTurnType())
	{
	case OptionsGame::TurnSequentialLooserFirst:
		gameType = "Sequential (Loser)";
		break;
	case OptionsGame::TurnSequentialRandom:
		gameType = "Sequential (Random)";
		break;
	case OptionsGame::TurnSimultaneous:
		gameType = "Simultaneous";
		break;
	}
	return gameType;
}

unsigned int OptionsTransient::getLeastUsedTeam(TankContainer &container)
{
	// Reset all the counts
	std::map<unsigned int, unsigned int> counts;
	for (int i=1; i<=optionsGame_.getTeams(); i++)
	{
		counts[i] = 0;
	}

	// Add all the tanks to the counts
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tanks = 
		container.getPlayingTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getTeam() > 0 &&
			tank->getState().getTankPlaying())
		{
			counts[tank->getTeam()] ++;
		}
	}

	// Find the least counted team
	unsigned int team = 1;
	unsigned int count = counts[1];
	for (int i=2; i<=optionsGame_.getTeams(); i++)
	{
		if (counts[i] < count)
		{
			team = i;
			count = counts[i];
		}
	}
	return team;
}

bool OptionsTransient::writeToBuffer(NetBuffer &buffer)
{
	if (!OptionEntryHelper::writeToBuffer(options_, buffer, false)) return false;
	return true;
}

bool OptionsTransient::readFromBuffer(NetBufferReader &reader)
{
	if (!OptionEntryHelper::readFromBuffer(options_, reader, false)) return false;
	return true;
}

void OptionsTransient::reset()
{
	currentGameNo_.setValue(0);
	currentRoundNo_.setValue(0);
}

void OptionsTransient::startNewGame()
{
	currentRoundNo_.setValue(optionsGame_.getNoRounds()+1);
}

void OptionsTransient::startNewRound()
{
	currentGameNo_.setValue(optionsGame_.getNoMaxRoundTurns() + 1);
}

void OptionsTransient::newGame()
{
	newGame_ = true;
	currentRoundNo_.setValue(currentRoundNo_.getValue() + 1);
	if (currentRoundNo_.getValue() >= optionsGame_.getBuyOnRound() &&
		!optionsGame_.getGiveAllWeapons())
	{
		currentGameNo_.setValue(0);	
	}
	else
	{
		currentGameNo_.setValue(1);
	}
	
	newGameWind();
	newGameWall();
}

void OptionsTransient::nextRound()
{
	if (!newGame_)
	{
		currentGameNo_.setValue(currentGameNo_.getValue() + 1);
	}
	newGame_ = false;
}

void OptionsTransient::newGameWind()
{
	FileRandomGenerator random;
	random.seed(rand());

	switch(optionsGame_.getWindForce())
	{
		case OptionsGame::WindRandom:
			windSpeed_.setValue(
				(random.getRandFixed() * fixed(true, 59000)).asInt()); // ie range 0->5
			break;
		case OptionsGame::Wind1:
		case OptionsGame::Wind2:
		case OptionsGame::Wind3:
		case OptionsGame::Wind4:
		case OptionsGame::Wind5:
			windSpeed_.setValue(
				fixed(int(optionsGame_.getWindForce()) - 1));
			break;
		case OptionsGame::WindBreezy:
			windSpeed_.setValue(
				(random.getRandFixed() * fixed(true, 29000)).asInt());// ie range 0->2);
			break;
		case OptionsGame::WindGale:
			windSpeed_.setValue(
				(random.getRandFixed() * fixed(true, 29000)).asInt() + 3); // ie range 3->5);
			break;
		case OptionsGame::WindNone:
		default:
			windSpeed_.setValue(0);
			break;
	}

	if (windSpeed_.getValue() > 0)
	{
		fixed winAngle = random.getRandFixed() * 360;
		windStartAngle_.setValue(winAngle);
		windAngle_.setValue(winAngle);
		
		fixed windDirX = (winAngle / fixed(180) * fixed::XPI).sin();
		fixed windDirY = (winAngle / fixed(180) * fixed::XPI).cos();
		FixedVector windDir(windDirX, windDirY, 0);
		windDirection_.setValue(windDir);
	}
	else
	{
		windStartAngle_.setValue(0);
		windAngle_.setValue(0);
		windDirection_.setValue(FixedVector::getNullVector());
	}
}

Vector &OptionsTransient::getWallColor()
{
	static Vector wallColor;
	switch (getWallType())
	{
	case wallWrapAround:
		wallColor = Vector(0.5f, 0.5f, 0.0f);
		break;
	case wallBouncy:
		wallColor = Vector(0.0f, 0.0f, 0.5f);
		break;
	case wallConcrete:
		wallColor = Vector(0.5f, 0.5f, 0.5f);
		break;
	default:
		wallColor = Vector(0.0f, 0.0f, 0.0f);
		break;
	}

	return wallColor;
}

void OptionsTransient::newGameWall()
{
	switch (optionsGame_.getWallType())
	{
	case OptionsGame::WallConcrete:
		wallType_.setValue((int) wallConcrete);
		break;
	case OptionsGame::WallBouncy:
		wallType_.setValue((int) wallBouncy);
		break;
	case OptionsGame::WallWrapAround:
		wallType_.setValue((int) wallWrapAround);
		break;
	case OptionsGame::WallNone:
		wallType_.setValue((int) wallNone);
		break;
	float r;
	case OptionsGame::WallActive:	// Bouncy or Wrap
		r = RAND * 2.0f + 1.0f;
		wallType_.setValue((int) r);
		break;
	case OptionsGame::WallInactive:	// Concrete or None
		if (RAND < 0.5f) wallType_.setValue((int) wallConcrete);
		else wallType_.setValue((int) wallNone);
		break;
	default:
		r = RAND * 4.0f;
		wallType_.setValue((int) r);
		break;
	}
}

int OptionsTransient::getArmsLevel()
{
	float start = (float) optionsGame_.getStartArmsLevel();
	float end = (float) optionsGame_.getEndArmsLevel();

	float roundsPlayed = float(getCurrentRoundNo());
	float totalRounds = float(optionsGame_.getNoRounds());

	float armsLevel = start + ((end - start) * (roundsPlayed / totalRounds));
	return (int) armsLevel;
}
