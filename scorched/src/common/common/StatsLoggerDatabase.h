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

#if !defined(__INCLUDE_StatsLoggerDatabaseh_INCLUDE__)
#define __INCLUDE_StatsLoggerDatabaseh_INCLUDE__

#include <common/StatsLogger.h>
#include <time.h>
#include <string>
#include <map>
#include <set>

class Weapon;
class StatsLoggerDatabase : public StatsLogger
{
public:
	StatsLoggerDatabase();
	virtual ~StatsLoggerDatabase();

	virtual int getKillCount(const char *uniqueId);
	virtual void gameStart(std::list<Tank *> &tanks);
	virtual void roundStart(std::list<Tank *> &tanks);

	virtual std::list<std::string> getAliases(const char *unqiueId);
	virtual std::list<std::string> getIpAliases(const char *unqiueId);
	virtual TankRank tankRank(Tank *tank);
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
	struct RowResult
	{
		std::vector<std::string> columns;
		std::map<std::string, unsigned int> names;

		const char *getValue(const char *name);
	};

	// Overridden by new implementations
	virtual bool runQuery(const char *, ...) = 0;
	virtual std::list<RowResult> runSelectQuery(const char *, ...) = 0;
	virtual bool connectDatabase(const char *host, const char *port,
		const char *user, const char *passwd, 
		const char *db) = 0;

	virtual int getLastInsertId() = 0;
	virtual void escapeString(char *to, const char *from, unsigned long length) = 0;

	// Default stuff
	time_t updateTime_;
	int serverid_;
	int seriesid_;
	int prefixid_;
    bool success_;
	bool displayStats_;

	std::map<std::string, int> playerId_;
	std::map<std::string, int> weaponId_;

	void createLogger();
	int getPlayerId(const char *uniqueId);

	void addInfo(Tank *tank);
	void addAliases(int playerId, 
		std::list<std::string> &results);
	void addIpAliases(int playerId, 
		std::set<int> &currentPlayers, std::list<std::string> &result);

};

#endif 

