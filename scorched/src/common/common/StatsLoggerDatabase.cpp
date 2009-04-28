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

#include <common/StatsLoggerDatabase.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <net/NetInterface.h>
#include <server/ServerCommon.h>
#include <weapons/AccessoryStore.h>
#include <server/ScorchedServer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankAvatar.h>
#include <XML/XMLFile.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

enum EventType
{
	EventKill = 1,
	EventTeamKill = 2,
	EventSelfKill = 3,
	EventResigned = 4,
	EventWon = 5,
	EventOverallWinner = 6,
	EventConnected = 7,
	EventDisconnected = 8,
	EventJoined = 9
};

const char *StatsLoggerDatabase::RowResult::getValue(const char *name)
{
	std::map<std::string, unsigned int>::iterator findItor =
		names.find(name);
	if (findItor == names.end()) return 0;
	unsigned int pos = findItor->second;
	return columns[pos].c_str();
}

StatsLoggerDatabase::StatsLoggerDatabase() : 
	success_(false),
	serverid_(0), seriesid_(0), prefixid_(0),
	updateTime_(0)
{
}

StatsLoggerDatabase::~StatsLoggerDatabase()
{
}

void StatsLoggerDatabase::createLogger()
{
	if (success_) return;

	XMLFile file;
	std::string fileName = S3D::getSettingsFile(S3D::formatStringBuffer("mysql-%i.xml",
		ScorchedServer::instance()->getOptionsGame().getPortNo()));

	std::string host, port, user, passwd, db, prefix;
	if (!file.readFile(fileName) ||
		!file.getRootNode())
	{
		S3D::dialogExit("Stats Logging",
			S3D::formatStringBuffer(
			"Failed to parse %s settings file. Error: %s", 
			fileName.c_str(),
			file.getParserError()));
		return;
	}

	if (!file.getRootNode()->getNamedChild("host", host) ||
		!file.getRootNode()->getNamedChild("port", port) ||
		!file.getRootNode()->getNamedChild("user", user) ||
		!file.getRootNode()->getNamedChild("passwd", passwd) ||
		!file.getRootNode()->getNamedChild("db", db) ||
		!file.getRootNode()->getNamedChild("prefix", prefix)) 
	{
		S3D::dialogExit("Stats Logging", 
			S3D::formatStringBuffer(
			"Failed to parse %s settings file.", fileName.c_str()));
		return;
	}

	// Create the database connection
	success_ = false;
	if (!connectDatabase(host.c_str(), port.c_str(), 
		user.c_str(), passwd.c_str(), 
		db.c_str()))
	{
		S3D::dialogExit("Stats Logging",
			"Failed to connect to stats database");
		return;
	}
	success_ = true;

	// Add event types
	runQuery("INSERT INTO scorched3d_eventtypes "
		"(eventtype, name) VALUES "
		"(%i, \"KILL\"), "
		"(%i, \"TEAMKILL\"), "
		"(%i, \"SELFKILL\"), "
		"(%i, \"WON\"), "
		"(%i, \"OVERALLWINNER\"), "
		"(%i, \"CONNECTED\"), "
		"(%i, \"DISCONNECTED\"), "
		"(%i, \"JOINED\"), "
		"(%i, \"RESIGNED\"); ",
		EventKill, EventTeamKill, EventSelfKill,
		EventWon, EventOverallWinner, 
		EventConnected, EventDisconnected, 
		EventJoined, EventResigned);

	// Get/allocate the prefixid
	std::list<StatsLoggerDatabase::RowResult> prefixRows =
		runSelectQuery("SELECT prefixid FROM scorched3d_prefixs "
		"WHERE prefix = \"%s\";",
		prefix.c_str());
	if (!prefixRows.empty())
	{
		std::list<StatsLoggerDatabase::RowResult>::iterator itor;
		for (itor = prefixRows.begin();
			itor != prefixRows.end();
			itor++)
		{
			StatsLoggerDatabase::RowResult &rowResult = (*itor);
			prefixid_ = atoi(rowResult.columns[0].c_str());
		}
	}
	if (prefixid_ == 0)
	{
		if (runQuery("INSERT INTO scorched3d_prefixs "
			"(prefix) VALUES(\"%s\");",
			prefix.c_str()))
		{
			prefixid_ = getLastInsertId();
		}
	}

	// Get/allocate the server id
	std::list<StatsLoggerDatabase::RowResult> serverIdRows =
		runSelectQuery("SELECT serverid, displaystats FROM scorched3d_servers "
		"WHERE name = \"%s\";",
		ScorchedServer::instance()->getOptionsGame().getServerName());
	if (!serverIdRows.empty())
	{
		std::list<StatsLoggerDatabase::RowResult>::iterator itor;
		for (itor = serverIdRows.begin();
			itor != serverIdRows.end();
			itor++)
		{
			StatsLoggerDatabase::RowResult &rowResult = (*itor);
			serverid_ = atoi(rowResult.columns[0].c_str());
			displayStats_ = (atoi(rowResult.columns[1].c_str()) != 0);
		}
	}
	if (serverid_ == 0)
	{
		if (runQuery("INSERT INTO scorched3d_servers "
			"(name, published) VALUES(\"%s\", \"%s\");",
			ScorchedServer::instance()->getOptionsGame().getServerName(),
			ScorchedServer::instance()->getOptionsGame().getPublishAddress()))
		{
			serverid_ = getLastInsertId();
			displayStats_ = true;
		}
	}

	// Get/allocate the series id
	std::list<StatsLoggerDatabase::RowResult> seriesIdRows =
		runSelectQuery("SELECT seriesid FROM scorched3d_series "
		"WHERE type = 0;");
	if (!seriesIdRows.empty())
	{
		std::list<StatsLoggerDatabase::RowResult>::iterator itor;
		for (itor = seriesIdRows.begin();
			itor != seriesIdRows.end();
			itor++)
		{
			StatsLoggerDatabase::RowResult &rowResult = (*itor);
			seriesid_ = atoi(rowResult.columns[0].c_str());
		}
	}
	if (seriesid_ == 0)
	{
		if (runQuery("INSERT INTO scorched3d_series "
			"(started, ended) VALUES(NOW(), NOW());"))
		{
			seriesid_ = getLastInsertId();
		}
	}

	// Add this server in this series
	runQuery("INSERT INTO scorched3d_statssource "
		"(serverid, prefixid, seriesid) VALUES "
		"(%i, %i, %i);", 
		serverid_, prefixid_, seriesid_);


	// Add all the weapons
	std::list<Accessory *> weapons = 
		ScorchedServer::instance()->getAccessoryStore().getAllAccessories();
	std::list<Accessory *>::iterator itor;	
	for (itor = weapons.begin();
		itor != weapons.end();
		itor++)
	{
		Accessory *accessory = *itor;

		int weaponId = 0;
		std::list<StatsLoggerDatabase::RowResult> weaponIdRows =
			runSelectQuery("SELECT weaponid FROM scorched3d_weapons "
				"WHERE name = \"%s\" AND seriesid = %i AND prefixid = %i;", 
				accessory->getName(),
				seriesid_,
				prefixid_);
		if (!weaponIdRows.empty())
		{
			std::list<StatsLoggerDatabase::RowResult>::iterator itor;
			for (itor = weaponIdRows.begin();
				itor != weaponIdRows.end();
				itor++)
			{
				StatsLoggerDatabase::RowResult &rowResult = (*itor);
				weaponId = atoi(rowResult.columns[0].c_str());
			}
		}

		if (weaponId == 0)
		{
			if (runQuery("INSERT INTO scorched3d_weapons "
				"(seriesid, prefixid, name, description, armslevel, cost, bundlesize, icon) "
				"VALUES(%i, %i, \"%s\", \"%s\", %i, %i, %i, \"%s\");", 
				seriesid_,
				prefixid_,
				accessory->getName(), 
				accessory->getDescription(),
				accessory->getArmsLevel(),
				accessory->getOriginalPrice(),
				accessory->getBundle(),
				accessory->getIconName()))
			{
				weaponId = getLastInsertId();
			}
		}
		else
		{
			runQuery("UPDATE scorched3d_weapons SET "
				"description = \"%s\", "
				"armslevel = %i, "
				"cost = %i, "
				"bundlesize = %i, "
				"icon = \"%s\" "
				"WHERE name = \"%s\" AND seriesid = %i AND prefixid = %i;", 
				accessory->getDescription(),
				accessory->getArmsLevel(),
				accessory->getOriginalPrice(),
				accessory->getBundle(),
				accessory->getIconName(),
				accessory->getName(),
				seriesid_,
				prefixid_);
		}

		weaponId_[accessory->getName()] = weaponId;
	}

	Logger::log(S3D::formatStringBuffer("database stats logger started, prefix=%i, server=%i, series=%i",
		prefixid_, serverid_, seriesid_));
	periodicUpdate();
}

void StatsLoggerDatabase::addIpAliases(int playerId, 
	std::set<int> &currentPlayers, std::list<std::string> &results)
{
	currentPlayers.insert(playerId);
	addAliases(playerId, results);

	std::list<std::string> ipaddresses;
	std::list<StatsLoggerDatabase::RowResult> ipaddressesRows =
		runSelectQuery("SELECT ipaddress FROM scorched3d_ipaddress "
			"WHERE playerid = %i;", playerId);
	if (!ipaddresses.empty())
	{
		std::list<StatsLoggerDatabase::RowResult>::iterator itor;
		for (itor = ipaddressesRows.begin();
			itor != ipaddressesRows.end();
			itor++)
		{
			StatsLoggerDatabase::RowResult &rowResult = (*itor);
			ipaddresses.push_back(rowResult.columns[0]);
		}
	}

	std::list<std::string>::iterator itor;
	for (itor = ipaddresses.begin();
		itor != ipaddresses.end();
		itor++)
	{
		const char *ipaddress = (*itor).c_str();
		std::list<int> newplayers;

		std::list<StatsLoggerDatabase::RowResult> ipaddressRows =
			runSelectQuery("SELECT playerid FROM scorched3d_ipaddress "
				"WHERE ipaddress = \"%s\";", ipaddress);
		if (!ipaddressRows.empty())
		{
			std::list<StatsLoggerDatabase::RowResult>::iterator itor;
			for (itor = ipaddressRows.begin();
				itor != ipaddressRows.end();
				itor++)
			{
				StatsLoggerDatabase::RowResult &rowResult = (*itor);
				int newplayerid = atoi(rowResult.columns[0].c_str());
				if (currentPlayers.find(newplayerid) == currentPlayers.end())
				{
					newplayers.push_back(newplayerid);
				}
			}
		}

		std::list<int>::iterator itor2;
		for (itor2 = newplayers.begin();
			itor2 != newplayers.end();
			itor2++)
		{
			addIpAliases((*itor2), currentPlayers, results);
		}
	}
}

std::string StatsLoggerDatabase::getTopRanks()
{
        createLogger();
        if (!success_) return "";

	const char *columns = 
		"rank, kills, deaths, selfkills, teamkills, shots, wins, "
		"overallwinner, resigns, gamesplayed, timeplayed, roundsplayed, "
		"moneyearned, skill, name";

	std::string stringResult;
	std::list<StatsLoggerDatabase::RowResult> rankRows =
		runSelectQuery(
			"select %s from scorched3d_stats "
			"left join scorched3d_players on scorched3d_stats.playerid = "
			"scorched3d_players.playerid where seriesid=%i and prefixid=%i "
			"order by skill desc limit 0,50",
			columns, seriesid_, prefixid_);

	stringResult.append("<table>");

	std::string cols(columns);
	char *token = strtok((char *) cols.c_str(), " ");
	stringResult.append("<tr>");
	while(token != 0)
	{
		stringResult.append("<td><b>").append(token).append("</b></td>");
		token = strtok(0, " ");
	}
	stringResult.append("</tr>");

	if (!rankRows.empty())
	{
		std::list<StatsLoggerDatabase::RowResult>::iterator itor;
		for (itor = rankRows.begin();
			itor != rankRows.end();
			itor++)
		{
			StatsLoggerDatabase::RowResult &result = (*itor);
			stringResult.append("<tr>");
			for (unsigned int i=0; i<result.columns.size(); i++)
			{
				stringResult.append("<td>").append(result.columns[i]).append("</td>");
			}
			stringResult.append("</tr>");
		}
	}
	stringResult.append("</table>");
	return S3D::formatStringBuffer("%s", stringResult.c_str());
}

std::string StatsLoggerDatabase::getPlayerInfo(const char *player)
{
        createLogger();
        if (!success_) return "";

	std::string stringResult;
	std::list<StatsLoggerDatabase::RowResult> playerRows =
		runSelectQuery("select "
			"scorched3d_names.playerid as playerid, "
			"scorched3d_names.name as name, "
			"uniqueid from "
			"scorched3d_names left join "
			"scorched3d_players on scorched3d_players.playerid = scorched3d_names.playerid "
			"where LOCATE(LOWER(\"%s\"), LOWER(scorched3d_names.name)) != 0 limit 0,50",
			player);
	if (!playerRows.empty())
	{
		std::list<StatsLoggerDatabase::RowResult>::iterator itor;
		for (itor = playerRows.begin();
			itor != playerRows.end();
			itor++)
		{
			StatsLoggerDatabase::RowResult &result = (*itor);
			for (unsigned int i=0; i<result.columns.size(); i++)
			{
				stringResult.append(result.columns[i]);
				if (i < result.columns.size() - 1) stringResult.append(",");
				else stringResult.append("\n");
			}
		}
	}
	
	return S3D::formatStringBuffer("%s", stringResult.c_str());
}

void StatsLoggerDatabase::combinePlayers(unsigned int player1, unsigned int player2)
{
        createLogger();
        if (!success_) return;

	// Check these players exist
	std::list<StatsLoggerDatabase::RowResult> player1Rows =
		runSelectQuery("select name, uniqueid from "
		"scorched3d_players where playerid=%i", 
		player1);
	std::list<StatsLoggerDatabase::RowResult> player2Rows =
		runSelectQuery("select name, uniqueid from "
		"scorched3d_players where playerid=%i", 
		player2);
	if (player1Rows.empty()) return;
	if (player2Rows.empty()) return;

	// Find all results from player2
	std::list<StatsLoggerDatabase::RowResult> player2Results =
		runSelectQuery("select * from scorched3d_stats where playerid=%i", 
		player2);
	std::list<StatsLoggerDatabase::RowResult>::iterator itor;
	for (itor = player2Results.begin();
		itor != player2Results.end();
		itor++)
	{
		StatsLoggerDatabase::RowResult &player2Result = *itor;
		const char *prefixId = player2Result.getValue("prefixid");
		const char *seriesId = player2Result.getValue("seriesid");
		if (prefixId && seriesId)
		{
			// Find all results from player1 in the same prefix and series
			std::list<StatsLoggerDatabase::RowResult> player1Results =
				runSelectQuery("select * from scorched3d_stats where playerid=%i AND "
				"prefixid=%s and seriesid=%s", 
				player1, prefixId, seriesId);
			if (player1Results.empty())
			{
				// There are no results for player1, just move the results
				runQuery("update scorched3d_stats set playerid=%i where "
					"playerid=%i and prefixid=%s and seriesid=%s", 
					player1, player2,
					prefixId, seriesId);
			}
			else
			{
				// There are results for player1, combine the results
				StatsLoggerDatabase::RowResult &player1Result = player1Results.front();
				std::map<std::string, unsigned int>::iterator itor;
				for (itor = player1Result.names.begin();
					itor != player1Result.names.end();
					itor++)
				{
					std::string name = itor->first;
					std::string value1 = player1Result.getValue(name.c_str());
					std::string value2 = player2Result.getValue(name.c_str());
					
					std::string query = "update scorched3d_stats ";
					if (name == "playerid" ||
						name == "prefixid" ||
						name == "seriesid" ||
						name == "lastconnected" ||
						name == "rank" ||
						name == "skill")
					{
						// Ignore
					}
					else
					{
						int v1 = atoi(value1.c_str());
						int v2 = atoi(value2.c_str());
						int value = v1 + v2;
						query.append(S3D::formatStringBuffer("set %s=%i ", name.c_str(), value));
					}
					query.append(
						S3D::formatStringBuffer("where playerid=%i and prefixid=%s and seriesid=%s", 
							player1, prefixId, seriesId));
					runQuery(query.c_str());
				}
			}
		}
	}

	// Remove player2
	runQuery("delete from scorched3d_players where playerid=%i", player2);
	runQuery("delete from scorched3d_stats where playerid=%i", player2);
	runQuery("delete from scorched3d_names where playerid=%i", player2);
	runQuery("delete from scorched3d_ipaddresses where playerid=%i", player2);
}

std::list<std::string> StatsLoggerDatabase::getIpAliases(const char *unqiueId)
{
	std::list<std::string> results;
	createLogger();
	if (!success_) return results;
	
	int playerId = getPlayerId(unqiueId);
	if (playerId == 0) return results;

	std::set<int> currentPlayers;
	addIpAliases(playerId, currentPlayers, results);
	
	return results;
}

static bool findInList(std::list<std::string> &results, 
	const char *name)
{
	std::list<std::string>::iterator itor;
	for (itor = results.begin();
		itor != results.end();
		itor++)
	{
		if (0 == strcmp(name, (*itor).c_str())) return true;
	}
	return false;
}

void StatsLoggerDatabase::addAliases(int playerId, 
	std::list<std::string> &results)
{
	// Make sure the last used name is in the list and
	// it is always first
	{
		std::list<StatsLoggerDatabase::RowResult> nameRows =
			runSelectQuery("SELECT name FROM scorched3d_players "
				"WHERE playerid = %i;", playerId);
		if (!nameRows.empty())
		{
			std::list<StatsLoggerDatabase::RowResult>::iterator itor;
			for (itor = nameRows.begin();
				itor != nameRows.end();
				itor++)
			{
				StatsLoggerDatabase::RowResult &rowResult = (*itor);
				if (!findInList(results, rowResult.columns[0].c_str()))
				{
					results.push_back(rowResult.columns[0].c_str());
				}
			}
		}
	}

	// Add all other UNIQUE aliases
	{
		std::list<StatsLoggerDatabase::RowResult> nameRows =
			runSelectQuery("SELECT name FROM scorched3d_names "
				"WHERE playerid = %i;", playerId);
		if (!nameRows.empty())
		{
			std::list<StatsLoggerDatabase::RowResult>::iterator itor;
			for (itor = nameRows.begin();
				itor != nameRows.end();
				itor++)
			{
				StatsLoggerDatabase::RowResult &rowResult = (*itor);
				if (!findInList(results, rowResult.columns[0].c_str()))
				{
					results.push_back(rowResult.columns[0].c_str());
				}
			}
		}
	}
}

std::list<std::string> StatsLoggerDatabase::getAliases(const char *unqiueId)
{
	std::list<std::string> results;
	createLogger();
	if (!success_) return results;
	
	int playerId = getPlayerId(unqiueId);
	if (playerId == 0) return results;
	addAliases(playerId, results);

	return results;
}

void StatsLoggerDatabase::gameStart(std::list<Tank *> &tanks)
{
	createLogger();
	if (!success_) return;

	runQuery("UPDATE scorched3d_series SET games = games + 1, "
		"ended = NOW() WHERE seriesid = %i;",
		seriesid_);

	std::list<Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = *itor;
		if (tank->getState().getTankPlaying())
		{
			runQuery("UPDATE scorched3d_stats SET gamesplayed=gamesplayed+1 "
				"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
				playerId_[tank->getUniqueId()],
				prefixid_,
				seriesid_);
		}
	}
}

void StatsLoggerDatabase::roundStart(std::list<Tank *> &tanks)
{
	createLogger();
	if (!success_) return;

	runQuery("UPDATE scorched3d_series SET rounds = rounds + 1, "
		"ended = NOW() WHERE seriesid = %i;",
		seriesid_);

	std::list<Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = *itor;
		if (tank->getState().getTankPlaying())
		{
			runQuery("UPDATE scorched3d_stats SET roundsplayed=roundsplayed+1 "
				"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
				playerId_[tank->getUniqueId()],
				prefixid_,
				seriesid_);
		}
	}
}

void StatsLoggerDatabase::tankFired(Tank *firedTank, Weapon *weapon)
{
	createLogger();
	if (!success_) return;

	runQuery("UPDATE scorched3d_stats SET shots=shots+1 "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId_[firedTank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void StatsLoggerDatabase::tankResigned(Tank *tank)
{
	createLogger();
	if (!success_) return;

    runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventResigned,
		playerId_[tank->getUniqueId()]);

	runQuery("UPDATE scorched3d_stats SET resigns=resigns+1 "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId_[tank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void StatsLoggerDatabase::updateStats(Tank *tank)
{
	createLogger();
	if (!success_) return;

	if (tank->getState().getTankPlaying())
	{
		unsigned int playerId = playerId_[tank->getUniqueId()];

		runQuery("UPDATE scorched3d_stats SET "
			"timeplayed=timeplayed+%i, moneyearned=moneyearned+%i, scoreearned=scoreearned+%i "
			"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
			((unsigned int) tank->getScore().getTimePlayedStat()),
			tank->getScore().getTotalMoneyEarnedStat(), 
			tank->getScore().getTotalScoreEarnedStat(),
			playerId,
			prefixid_,
			seriesid_);
	}
}

void StatsLoggerDatabase::periodicUpdate()
{
	time_t currentTime = time(0);
	if (currentTime - updateTime_ > 60 * 60 * 12) // 12 hrs
	{
		updateTime_ = currentTime;
		Logger::log(S3D::formatStringBuffer("statslogger database starting periodics"));

		// Cleanup orphaned avatars
		std::list<StatsLoggerDatabase::RowResult> binaryRows =
			runSelectQuery("select binaryid, count(binaryid) "
				"from scorched3d_binary left join "
				"scorched3d_players on binaryid = avatarid group by avatarid");
		if (!binaryRows.empty())
		{
			std::list<StatsLoggerDatabase::RowResult>::iterator itor;
			for (itor = binaryRows.begin();
				itor != binaryRows.end();
				itor++)
			{
				StatsLoggerDatabase::RowResult &rowResult = (*itor);
				if (rowResult.columns[1] == "0")
				{
					runQuery("delete from scorched3d_binary where binaryid = %s",
						rowResult.columns[0].c_str());
				}
			}
		}

		// Generate ranks
		std::list<StatsLoggerDatabase::RowResult>::iterator playerItor;
		std::list<StatsLoggerDatabase::RowResult> playerRows =
			runSelectQuery(
				"SELECT playerid, skill from scorched3d_stats "
				"WHERE seriesid=%u and prefixid=%u order by skill desc",
				seriesid_,
				prefixid_);

		int rank = 1;
		for (playerItor = playerRows.begin();
			playerItor != playerRows.end();
			playerItor++, rank++)
		{
			StatsLoggerDatabase::RowResult &playerRow = *playerItor;
			runQuery("UPDATE scorched3d_stats SET rank=%i "
				"WHERE seriesid=%u and prefixid=%u and playerid=%s",
				rank, 
				seriesid_,
				prefixid_,
				playerRow.columns[0].c_str());
		}

		Logger::log(S3D::formatStringBuffer("statslogger database finished periodics"));
	}
}

StatsLogger::TankRank StatsLoggerDatabase::tankRank(Tank *tank)
{
	TankRank result;

	createLogger();
	if (!success_ || !displayStats_) return result;

	// Try to determine this players sql playerid
	std::list<StatsLoggerDatabase::RowResult> skillRows =
		runSelectQuery("SELECT skill FROM scorched3d_stats "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId_[tank->getUniqueId()],
		prefixid_,
		seriesid_);
	if (!skillRows.empty())
	{
		std::list<StatsLoggerDatabase::RowResult>::iterator itor;
		for (itor = skillRows.begin();
			itor != skillRows.end();
			itor++)
		{
			StatsLoggerDatabase::RowResult &rowResult = (*itor);
			result.skill = atoi(rowResult.columns[0].c_str());
		}

		std::list<StatsLoggerDatabase::RowResult> countRows =
			runSelectQuery("SELECT count(*) FROM scorched3d_stats "
			"WHERE skill > \"%i\" AND prefixid = %i AND seriesid = %i;", 
			result.skill,
			prefixid_,
			seriesid_);
		if (!countRows.empty())
		{
			std::list<StatsLoggerDatabase::RowResult>::iterator itor;
			for (itor = countRows.begin();
				itor != countRows.end();
				itor++)
			{
				StatsLoggerDatabase::RowResult &rowResult = (*itor);
				result.rank = atoi(rowResult.columns[0].c_str()) + 1;
			}
		}
	}

	return result;
}

int StatsLoggerDatabase::getPlayerId(const char *uniqueId)
{
	createLogger();
	if (!success_) return 0;

	// Try to determine this players sql playerid
	int playerId = 0;
	std::list<StatsLoggerDatabase::RowResult> playerIdRows =
		runSelectQuery("SELECT playerid FROM scorched3d_players "
			"WHERE uniqueid = \"%s\";", uniqueId);
	if (!playerIdRows.empty())
	{
		std::list<StatsLoggerDatabase::RowResult>::iterator itor;
		for (itor = playerIdRows.begin();
			itor != playerIdRows.end();
			itor++)
		{
			StatsLoggerDatabase::RowResult &rowResult = (*itor);
			playerId = atoi(rowResult.columns[0].c_str());
		}
	}

	return playerId;
}

unsigned int StatsLoggerDatabase::getStatsId(const char *uniqueId)
{
	createLogger();
	if (!success_) return 0;
	int id = getPlayerId(uniqueId);
	return id;
}

std::string StatsLoggerDatabase::allocateId() 
{ 
	const char possibleChars [] = {
		'1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S',
		'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
	char buffer[128];

	do
	{
		int pos = 0;
		for (int j=0; j<3; j++)
		{
			for (int i=0; i<8; i++)
			{
				buffer[pos++] = possibleChars[rand() % 33];
			}
			buffer[pos++] = '-';
		}
		buffer[pos - 1] = '\0';

	} while (getPlayerId(buffer) != 0);
	return buffer;
}

void StatsLoggerDatabase::addInfo(Tank *tank)
{
	char playerName[1024];
	escapeString(playerName, 
		tank->getCStrName().c_str(),
		(unsigned long) tank->getCStrName().size());

	// Add the players name (may fail if duplicates)
	runQuery("INSERT INTO scorched3d_names (playerid, name, count) VALUES "
		"(%i, \"%s\", 0);", 
		playerId_[tank->getUniqueId()], 
		playerName);
	runQuery("UPDATE scorched3d_names SET count=count+1 WHERE "
		"playerid=%i AND name=\"%s\";", 
		playerId_[tank->getUniqueId()], 
		playerName);

	// Add the ipaddress (may fail if duplicates)
	runQuery("INSERT INTO scorched3d_ipaddress (playerid, ipaddress, count) VALUES "
		"(%i, \"%s\", 0);",
		playerId_[tank->getUniqueId()], 
		NetInterface::getIpName(tank->getIpAddress()));
	runQuery("UPDATE scorched3d_ipaddress SET count=count+1 WHERE "
		"playerid=%i AND ipaddress=\"%s\";",
		playerId_[tank->getUniqueId()], 
		NetInterface::getIpName(tank->getIpAddress()));

	// Update last username etc
	runQuery("UPDATE scorched3d_players SET "
		"name=\"%s\", ipaddress=\"%s\" "
		"WHERE playerid = %i;",
		playerName, 
		NetInterface::getIpName(tank->getIpAddress()),
		playerId_[tank->getUniqueId()]);
}

void StatsLoggerDatabase::tankConnected(Tank *tank)
{
        createLogger();
        if (!success_) return;

	// We don't have a player id, create one
	int playerId = getPlayerId(tank->getUniqueId());
	if (playerId == 0)
	{
		if (runQuery("INSERT INTO scorched3d_players (uniqueid) "
			"VALUES(\"%s\");",
			tank->getUniqueId()))
		{
			playerId = getLastInsertId();
			Logger::log(S3D::formatStringBuffer("Add new stats user \"%i\"", playerId));
		}
	}
	else
	{
		Logger::log(S3D::formatStringBuffer("Found stats user \"%i\"", playerId));
	}

	// Create the players stats entry if it does not exist
	std::list<StatsLoggerDatabase::RowResult> playerIdRows =
		runSelectQuery("SELECT playerid FROM scorched3d_stats "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId,
		prefixid_,
		seriesid_);
	if (playerIdRows.empty())
	{
		runQuery("INSERT INTO scorched3d_stats (playerid, prefixid, seriesid) "
			"VALUES(%i, %i, %i);",
			playerId,
			prefixid_,
			seriesid_);
	}

	// Store this new player id
	playerId_[tank->getUniqueId()] = playerId;

	// Add name and ip address
	addInfo(tank);

	// Connecting events
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventConnected,
		playerId_[tank->getUniqueId()]);

	// Connecting stats
	runQuery("UPDATE scorched3d_players SET osdesc=\"%s\" "
		"WHERE playerid = %i;", 
		tank->getHostDesc(),
		playerId);
	runQuery("UPDATE scorched3d_stats SET connects=connects+1, "
		"lastconnected=NOW() "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId,
		prefixid_,
		seriesid_);

	TankRank rank = StatsLogger::instance()->tankRank(tank);
    tank->getScore().setRank(rank.rank);
    tank->getScore().setSkill(rank.skill);
}

void StatsLoggerDatabase::tankJoined(Tank *tank)
{
	createLogger();
	if (!success_) return;

	// Joined events
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventJoined,
		playerId_[tank->getUniqueId()]);

	// add new info
	addInfo(tank);

	// Add the avatar
	if (tank->getAvatar().getName()[0])
	{
		char buffer[32];
		for (int i=0; i<30; i++)
		{
			buffer[i] = tank->getAvatar().getName()[i];
			if (!buffer[i]) break;
		}
		buffer[30] = '\0';

		int binaryid = 0;
		unsigned int crc = tank->getAvatar().getCrc();
		std::list<StatsLoggerDatabase::RowResult> binaryIdRows =
			runSelectQuery("SELECT binaryid FROM scorched3d_binary "
			"WHERE name = \"%s\" AND crc = %u;", 
			buffer,
			crc);
		if (!binaryIdRows.empty())
		{
			std::list<StatsLoggerDatabase::RowResult>::iterator itor;
			for (itor = binaryIdRows.begin();
				itor != binaryIdRows.end();
				itor++)
			{
				StatsLoggerDatabase::RowResult &rowResult = (*itor);
				binaryid = atoi(rowResult.columns[0].c_str());
			}
		}

		if (binaryid == 0)
		{
			char *to = new char[tank->getAvatar().getFile().getBufferUsed() * 2];
            escapeString(to, 
				tank->getAvatar().getFile().getBuffer(),
				tank->getAvatar().getFile().getBufferUsed());
			if (runQuery("INSERT INTO scorched3d_binary "
				"(name, crc, length, data) "
				"VALUES(\"%s\", %u, %u, \"%s\");", 
				buffer,
				crc,
				tank->getAvatar().getFile().getBufferUsed(),
				to))
			{
				binaryid = getLastInsertId();
			}

			delete [] to;
		}

		// Set the avatar id
		runQuery("UPDATE scorched3d_players SET avatarid = %i "
			"WHERE playerid = %i;", 
			binaryid,
			playerId_[tank->getUniqueId()]);
	}
}

int StatsLoggerDatabase::getKillCount(const char *uniqueId)
{
        createLogger();
        if (!success_) return 0;

	int kills = 0;
	int playerId = getPlayerId(uniqueId);
	if (playerId != 0) 
	{
		std::list<StatsLoggerDatabase::RowResult> killsRows =
			runSelectQuery("SELECT kills FROM scorched3d_stats "
			"WHERE playerid = %i;",
			playerId);
		if (!killsRows.empty())
		{
			std::list<StatsLoggerDatabase::RowResult>::iterator itor;
			for (itor = killsRows.begin();
				itor != killsRows.end();
				itor++)
			{
				StatsLoggerDatabase::RowResult &rowResult = (*itor);
				kills += atoi(rowResult.columns[0].c_str());
			}
		}
	}

	return kills;
}

void StatsLoggerDatabase::tankDisconnected(Tank *tank)
{
	createLogger();
	if (!success_) return;

	// Leaving events
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventDisconnected,
		playerId_[tank->getUniqueId()]);

	updateStats(tank);
}

void StatsLoggerDatabase::tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	createLogger();
	if (!success_) return;

	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, %i, %i, NOW());",
		prefixid_, seriesid_,
		EventKill,
		playerId_[firedTank->getUniqueId()], 
		playerId_[deadTank->getUniqueId()], 
		weaponId_[weapon->getParent()->getName()]);

	runQuery("UPDATE scorched3d_stats SET kills=kills+1, skill=%i "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		firedTank->getScore().getSkill(),
		playerId_[firedTank->getUniqueId()],
		prefixid_,
		seriesid_);
		
	runQuery("UPDATE scorched3d_stats SET deaths=deaths+1, skill=%i "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		deadTank->getScore().getSkill(),
		playerId_[deadTank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void StatsLoggerDatabase::tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	createLogger();
	if (!success_) return;

	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, %i, %i, NOW());",
		prefixid_, seriesid_,
		EventTeamKill,
		playerId_[firedTank->getUniqueId()],
		playerId_[deadTank->getUniqueId()], 
		weaponId_[weapon->getParent()->getName()]);

	runQuery("UPDATE scorched3d_stats SET teamkills=teamkills+1, skill=%i  "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		firedTank->getScore().getSkill(),
		playerId_[firedTank->getUniqueId()],
		prefixid_,
		seriesid_);
		
	runQuery("UPDATE scorched3d_stats SET deaths=deaths+1 "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId_[deadTank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void StatsLoggerDatabase::tankSelfKilled(Tank *firedTank, Weapon *weapon)
{
	createLogger();
	if (!success_) return;

	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, %i, NOW());",
		prefixid_, seriesid_, 
		EventSelfKill,
		playerId_[firedTank->getUniqueId()],
		weaponId_[weapon->getParent()->getName()]);

	runQuery("UPDATE scorched3d_stats SET selfkills=selfkills+1, deaths=deaths+1, skill=%i "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		firedTank->getScore().getSkill(),
		playerId_[firedTank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void StatsLoggerDatabase::tankWon(Tank *tank)
{
	createLogger();
	if (!success_) return;

	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventWon,
		playerId_[tank->getUniqueId()]);

	runQuery("UPDATE scorched3d_stats SET wins=wins+1, skill=%i  "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		tank->getScore().getSkill(),
		playerId_[tank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void StatsLoggerDatabase::tankOverallWinner(Tank *tank)
{
	createLogger();
	if (!success_) return;

	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventOverallWinner,
		playerId_[tank->getUniqueId()]);

	runQuery("UPDATE scorched3d_stats SET overallwinner=overallwinner+1, skill=%i "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;",
		tank->getScore().getSkill(),
		playerId_[tank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void StatsLoggerDatabase::weaponFired(Weapon *weapon, bool deathAni)
{
        createLogger();
        if (!success_) return;

	if (deathAni)
	{
		runQuery("UPDATE scorched3d_weapons SET deathshots=deathshots+1 "
			"WHERE weaponid = \"%i\" AND prefixid = %i AND seriesid = %i;",
			weaponId_[weapon->getParent()->getName()],
			prefixid_,
			seriesid_);
	}
	else
	{
		runQuery("UPDATE scorched3d_weapons SET shots=shots+1 "
			"WHERE weaponid = \"%i\" AND prefixid = %i AND seriesid = %i;", 
			weaponId_[weapon->getParent()->getName()],
			prefixid_,
			seriesid_);
	}
}

void StatsLoggerDatabase::weaponKilled(Weapon *weapon, bool deathAni)
{
        createLogger();
        if (!success_) return;

	if (deathAni)
	{
		runQuery("UPDATE scorched3d_weapons SET deathkills=deathkills+1 "
			"WHERE weaponid = \"%i\" AND prefixid = %i AND seriesid = %i;", 
			weaponId_[weapon->getParent()->getName()],
			prefixid_,
			seriesid_);
	}
	else
	{
		runQuery("UPDATE scorched3d_weapons SET kills=kills+1 "
			"WHERE weaponid = \"%i\" AND prefixid = %i AND seriesid = %i;", 
			weaponId_[weapon->getParent()->getName()],
			prefixid_,
			seriesid_);
	}
}

