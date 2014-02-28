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

#include <common/Defines.h>
#include <common/Logger.h>
#include <common/LoggerI.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <algorithm>

// ************************************************
// NOTE: This logger is and needs to be thread safe
// ************************************************

static boost::mutex logMutex_;
std::list<Logger::LoggerDefinition> Logger::loggers_;
std::list<LoggerInfo *> Logger::entries_;

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::addLogger(LoggerI *logger, bool logFromUIThread)
{
	logMutex_.lock();
	loggers_.push_back(LoggerDefinition(logger, logFromUIThread));
	logMutex_.unlock();
}

void Logger::remLogger(LoggerI *logger)
{
	std::list<LoggerDefinition>::iterator itor;
	logMutex_.lock();
	itor = loggers_.begin();
	while(itor != loggers_.end()) {
		if (itor->logger_ == logger) {
			loggers_.erase(itor);
			break;
		}
		++itor;
	}
	logMutex_.unlock();
}

void Logger::log(const std::string &text)
{
	log(text.c_str());
}

void Logger::log(const char *text)
{
	if (!text) return;

	LoggerInfo info;
	info.setMessage(text);
	log(info);
}

void Logger::log(const LoggerInfo &info)
{
	addLog((LoggerInfo &) info);
}

void Logger::addLog(LoggerInfo &info)
{
	// Add the time to the beginning of the log message
	info.setTime();

	// Add single or multiple lines
	char *found = (char *) strchr(info.getMessage(), '\n');
	char *start = (char *) info.getMessage();
	if (found)
	{
		while (found)
		{
			*found = '\0';
			LoggerInfo newInfo(info);
			newInfo.setMessage(start);
			addLogEntry(newInfo);
			start = found;
			start++;

			found = strchr(start, '\n');
		}
		if (start[0] != '\0')
		{
			LoggerInfo newInfo(info);
			newInfo.setMessage(start);
			addLogEntry(newInfo);
		}
	}
	else
	{
		addLogEntry(info);
	}
}

void Logger::addLogEntry(LoggerInfo &info)
{
	logMutex_.lock();

	bool entryRequired = false;
	std::list<LoggerDefinition>::iterator logItor;
	std::list<LoggerDefinition>::iterator endItor = loggers_.end();
	for (logItor = loggers_.begin();
		logItor != endItor;
		++logItor)
	{
		LoggerDefinition &log = (*logItor);
		if (!log.logFromUIThread_)
		{
			log.logger_->logMessage(info);
		}
		else
		{
			entryRequired = true;
		}
	}
	if (entryRequired)
	{
		entries_.push_back(new LoggerInfo(info));
	}
	
	logMutex_.unlock();
}

void Logger::processLogEntries()
{
	logMutex_.lock();
	while (!entries_.empty())
	{
		LoggerInfo *firstEntry = entries_.front();

		std::list<LoggerDefinition>::iterator logItor;
		std::list<LoggerDefinition>::iterator endItor = loggers_.end();
		for (logItor = loggers_.begin();
			logItor != endItor;
			++logItor)
		{
			LoggerDefinition &log = (*logItor);
			if (log.logFromUIThread_)
			{
				log.logger_->logMessage(*firstEntry);
			}
		}

		entries_.pop_front();
		delete firstEntry;
	}
	logMutex_.unlock();
}

