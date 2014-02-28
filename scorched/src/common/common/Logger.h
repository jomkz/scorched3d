////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <common/DefinesString.h>
#include <list>

// ************************************************
// NOTE: This logger is and needs to be thread safe
// ************************************************

class LoggerInfo;
class LoggerI;
class Logger
{
public:
	static LoggerInfo defaultInfo;
	static void addLogger(LoggerI *logger, bool logFromUIThread);
	static void remLogger(LoggerI *logger);
	static void processLogEntries();

	static void log(const LoggerInfo &info);
	static void log(const char *text);
	static void log(const std::string &text);

protected:
	class LoggerDefinition
	{
	public:
		LoggerDefinition(LoggerI *logger, bool logFromUIThread) :
			logger_(logger), logFromUIThread_(logFromUIThread)
		{
		}

		LoggerI *logger_;
		bool logFromUIThread_;
	};

	static std::list<LoggerDefinition> loggers_;
	static std::list<LoggerInfo *> entries_;

	static void addLog(LoggerInfo &info);
	static void addLogEntry(LoggerInfo &info);

private:
	Logger();
	virtual ~Logger();
};

#endif /* _LOGGER_H_ */
