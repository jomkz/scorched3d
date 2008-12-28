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

#if !defined(__INCLUDE_StatsLoggerh_INCLUDE__)
#define __INCLUDE_StatsLoggerh_INCLUDE__

#include <tank/Tank.h>
#include <weapons/Weapon.h>
#include <list>

class StatsLogger
{
public:
	static StatsLogger *instance();

	struct TankRank
	{
		TankRank() : rank(-1), skill(0) {}

		int rank;
		int skill;
	};

	StatsLogger();
	virtual ~StatsLogger();

	virtual int getKillCount(const char *uniqueId) = 0;
	virtual void gameStart(std::list<Tank *> &tanks) = 0;
	virtual void roundStart(std::list<Tank *> &tanks) = 0;

	virtual std::list<std::string> getAliases(const char *unqiueId) = 0;
	virtual std::list<std::string> getIpAliases(const char *unqiueId) = 0;
	virtual TankRank tankRank(Tank *tank) = 0;
	virtual void updateStats(Tank *tank) = 0;
	virtual void periodicUpdate() = 0;
	virtual std::string allocateId() = 0;
	virtual unsigned int getStatsId(const char *uniqueId) = 0;
	virtual std::string getTopRanks() = 0;
	virtual std::string getPlayerInfo(const char *player) = 0;
	virtual void combinePlayers(unsigned int player1, unsigned int player2) = 0;

	virtual void tankConnected(Tank *tank) = 0;
	virtual void tankDisconnected(Tank *tank) = 0;
	virtual void tankJoined(Tank *tank) = 0;

	virtual void tankFired(Tank *firedTank, Weapon *weapon) = 0;
	virtual void tankResigned(Tank *resignedTank) = 0;

	virtual void tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon) = 0;
	virtual void tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon) = 0;
	virtual void tankSelfKilled(Tank *firedTank, Weapon *weapon) = 0;

	virtual void tankWon(Tank *tank) = 0;
	virtual void tankOverallWinner(Tank *tank) = 0;

	virtual void weaponFired(Weapon *weapon, bool deathAni) = 0;
	virtual void weaponKilled(Weapon *weapon, bool deathAni) = 0;

protected:
	static StatsLogger *instance_;

};

class StatsLoggerNone : public StatsLogger
{
public:
	StatsLoggerNone() {}
	virtual ~StatsLoggerNone() {}

	virtual int getKillCount(const char *uniqueId) { return 0; }
	virtual void gameStart(std::list<Tank *> &tanks) {}
	virtual void roundStart(std::list<Tank *> &tanks) {}

	virtual std::list<std::string> getAliases(const char *unqiueId) 
		{ std::list<std::string> result; return result; }
	virtual std::list<std::string> getIpAliases(const char *unqiueId) 
		{ std::list<std::string> result; return result; }
	virtual TankRank tankRank(Tank *tank) { return TankRank(); }
	virtual void updateStats(Tank *tank) {}
	virtual void periodicUpdate() {}
	virtual std::string allocateId() { return ""; }
	virtual unsigned int getStatsId(const char *uniqueId) { return 0; }
	virtual std::string getTopRanks() { return ""; }
	virtual std::string getPlayerInfo(const char *player) { return ""; }
	virtual void combinePlayers(unsigned int player1, unsigned int player2) {}

	virtual void tankConnected(Tank *tank) {}
	virtual void tankDisconnected(Tank *tank) {}
	virtual void tankJoined(Tank *tank) {}

	virtual void tankFired(Tank *firedTank, Weapon *weapon) {}
	virtual void tankResigned(Tank *resignedTank) {}

	virtual void tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon) {}
	virtual void tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon) {}
	virtual void tankSelfKilled(Tank *firedTank, Weapon *weapon) {}

	virtual void tankWon(Tank *tank) {}
	virtual void tankOverallWinner(Tank *tank) {}

	virtual void weaponFired(Weapon *weapon, bool deathAni) {}
	virtual void weaponKilled(Weapon *weapon, bool deathAni) {}
};

#endif

