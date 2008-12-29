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

#if !defined(__INCLUDE_OptionsTransienth_INCLUDE__)
#define __INCLUDE_OptionsTransienth_INCLUDE__

#include <common/Vector.h>
#include <common/OptionsScorched.h>
#include <net/NetBuffer.h>

class TankContainer;
class OptionsTransient
{
public:
	OptionsTransient(OptionsScorched &optionsGame);
	virtual ~OptionsTransient();

	const char *getGameType();
	unsigned int getLeastUsedTeam(TankContainer &container);

	enum WallType
	{
		wallConcrete = 0,
		wallBouncy = 1,
		wallWrapAround = 2,
		wallNone = 3
	};
	enum WallSide
	{
		LeftSide = 0,
		RightSide,
		TopSide,
		BotSide
	};

	void reset();
	void newGame();
	void nextRound();

	void startNewGame();
	void startNewRound();

	// Walls
	WallType getWallType() { return (WallType) wallType_.getValue(); }
	Vector &getWallColor();

	// Rounds left
	int getCurrentRoundNo() { return currentRoundNo_.getValue(); }
	int getCurrentGameNo() { return currentGameNo_.getValue(); }

	// Wind
	fixed getWindAngle() { return windAngle_.getValue(); }
	fixed getWindSpeed() { return windSpeed_.getValue(); }
	FixedVector &getWindDirection() { return windDirection_.getValue(); }
	bool getWindOn() { return (windSpeed_.getValue() > fixed(0)); }

	// Arms Level
	int getArmsLevel();

	// Used to send this structure over coms
	bool writeToBuffer(NetBuffer &buffer);
	bool readFromBuffer(NetBufferReader &reader);

protected:
	std::list<OptionEntry *> options_;
	OptionsScorched &optionsGame_;
	OptionEntryInt currentRoundNo_;
	OptionEntryInt currentGameNo_;
	OptionEntryFixed windAngle_;
	OptionEntryFixed windStartAngle_;
	OptionEntryFixed windSpeed_;
	OptionEntryFixedVector windDirection_;
	OptionEntryInt wallType_;

	bool newGame_;
	void newGameWind();
	void newGameWall();
	void nextRoundWind();

};

#endif
