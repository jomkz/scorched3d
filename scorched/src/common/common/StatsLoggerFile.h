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

#if !defined(__INCLUDE_StatsLoggerFileh_INCLUDE__)
#define __INCLUDE_StatsLoggerFileh_INCLUDE__

#include <common/FileLogger.h>
#include <common/StatsLogger.h>

class StatsLoggerFile : public StatsLogger
{
public:
	StatsLoggerFile();
	virtual ~StatsLoggerFile();

	virtual void gameStart(std::list<Tank *> &tanks);
	virtual void roundStart(std::list<Tank *> &tanks);

	virtual int getKillCount(const char *uniqueId);
	virtual std::list<std::string> getAliases(const char *unqiueId);
	virtual std::list<std::string> getIpAliases(const char *unqiueId);
	virtual TankRank tankRank(Tank *tank) { return TankRank(); }
	virtual void updateStats(Tank *tank);
	virtual void periodicUpdate();
	virtual std::string allocateId();
	virtual unsigned int getStatsId(const char *uniqueId);
	virtual std::string getTopRanks();
	virtual std::string getPlayerInfo(const char *player);
	virtual void combinePlayers(unsigned int player1, unsigned int player2);

	virtual void tankConnected(Tank *tank);
	virtual void tankDisconnected(Tank *tank);
	virtual void tankJoined(Tank *tank);

	virtual void tankFired(Tank *firedTank, Weapon *weapon);
	virtual void tankResigned(Tank *resignedTank);

	virtual void tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon);
	virtual void tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon);
	virtual void tankSelfKilled(Tank *firedTank, Weapon *weapon);

	virtual void tankWon(Tank *tank);
	virtual void tankOverallWinner(Tank *tank);

	virtual void weaponFired(Weapon *weapon, bool deathAni);
	virtual void weaponKilled(Weapon *weapon, bool deathAni);

protected:
	FileLogger *statsLogger_;

	void createLogger();

};

#endif
