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

#if !defined(__INCLUDE_ShotCountDownh_INCLUDE__)
#define __INCLUDE_ShotCountDownh_INCLUDE__

#include <engine/GameStateI.h>
#include <common/Fixed.h>

class ShotCountDown : public GameStateI
{
public:
	static ShotCountDown *instance();

	enum TimerType
	{
		ePlaying,
		eBuying
	};

	void showMoveTime(fixed timer, TimerType type, unsigned int playerId);
	void showRoundTime(fixed timer);

	void hideMoveTime();
	void hideRoundTime();

	void simulateTime(fixed simTime);

	//Inherited from GameStateI
	virtual void draw(const unsigned state);

protected:
	static ShotCountDown *instance_;

	struct MoveInfo
	{
		fixed timer_;
		TimerType type_;
		unsigned int playerId_;
		bool show_;
	} move;
	struct RoundInfo
	{
		fixed timer_;
		bool show_;
	} round;

	void drawMove();
	void drawRound();

private:
	ShotCountDown();
	virtual ~ShotCountDown ();
};

#endif
