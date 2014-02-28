////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#if !defined(__INCLUDE_TanketShotInfoh_INCLUDE__)
#define __INCLUDE_TanketShotInfoh_INCLUDE__

#include <net/NetBuffer.h>
#include <vector>

class Tanket;
class ScorchedContext;
class TanketShotInfo
{
public:
	TanketShotInfo(ScorchedContext &context);
	virtual ~TanketShotInfo();

	void setTanket(Tanket *tanket) { tanket_ = tanket; }

	// State change
	void newGame();

	// Power of gun
	fixed getMaxOverallPower();
	fixed getMaxPower() { return maxPower_; }
	void setMaxPower(fixed power);

	// Move stuff
	void setMoveId(unsigned int moveId) { moveId_ = moveId; }
	unsigned int getMoveId() { return moveId_; }
	int getMissedMoves() { return missedMoves_; }
	void setMissedMoves(int miss) { missedMoves_ = miss; }
	void setSkippedShots(int skipped) { skippedShots_ = skipped; }
	int getSkippedShots() { return skippedShots_; }
	void setUseNormalMoves(bool useNormalMoves) { useNormalMoves_ = useNormalMoves; }
	bool getUseNormalMoves() { return useNormalMoves_; }

	// Serialize the tank
	bool writeMessage(NamedNetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	ScorchedContext &context_;
	Tanket *tanket_;

	bool useNormalMoves_;
	unsigned int moveId_;
	int missedMoves_;
	int skippedShots_;
	fixed maxPower_;
};

#endif

