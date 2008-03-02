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

#if !defined(__INCLUDE_TurnControllerh_INCLUDE__)
#define __INCLUDE_TurnControllerh_INCLUDE__

#include <list>

class TurnController
{
public:
	static TurnController *instance();

	void newGame();
	void nextRound();
	void nextShot();
	void nextTurn();

	bool playerThisTurn(unsigned int playerId);
	std::list<unsigned int> &getPlayersThisShot() { return playersThisShot_; }
	std::list<unsigned int> &getPlayersThisTurn() { return playersThisTurn_; }
	std::list<unsigned int> &getPlayersLeftToMove() { return playersLeftToMove_; }

protected:
	static TurnController *instance_;

	std::list<unsigned int> playerOrder_; // Generated on new game
	std::list<unsigned int> playersLeftToMove_; // Generated on next round
	std::list<unsigned int> playersThisShot_; // Generated on next shot
	std::list<unsigned int> playersThisTurn_; // Generated on next turn

private:
	TurnController();
	virtual ~TurnController();
};

#endif
