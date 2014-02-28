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

#include <common/FileLogger.h>
#include <common/Defines.h>
#include <time.h>

FileLogger::FileLogger(const std::string &fileName, const std::string &directory, bool rotateLogs) : 
	fileName_(fileName), fileDirectory_(directory), 
	size_(0), logFile_(0), fileCount_(0),
	rotateLogs_(rotateLogs)
{

}

FileLogger::~FileLogger()
{
}

void FileLogger::logMessage(LoggerInfo &info)
{
	const unsigned int MaxSize = 256000;
	if (!logFile_ || (size_>MaxSize)) openFile();
	if (!logFile_) return;

	// Log to file and flush file
	if (rotateLogs_)
	{
		size_ += (unsigned int) strlen(info.getMessage());
	}
	fprintf(logFile_, "%s - %s\n", info.getTime(), info.getMessage());
	fflush(logFile_);
}

void FileLogger::openFile()
{
	size_ = 0;
	if (logFile_) fclose(logFile_);

	std::string logFileName;
	if (rotateLogs_)
	{
		time_t theTime = time(0);
		struct tm *newtime = localtime(&theTime); 

		logFileName = 
			S3D::formatStringBuffer("%s/%s-%i%02i%02i-%02i%02i%02i-%u.log", fileDirectory_.c_str(), fileName_.c_str(),
				newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday,
				newtime->tm_hour, newtime->tm_min, newtime->tm_sec, fileCount_);
		fileCount_++;		
	} 
	else
	{
		logFileName =
			S3D::formatStringBuffer("%s/%s", fileDirectory_.c_str(), fileName_.c_str());
	}

	logFile_ = fopen(logFileName.c_str(), "w");
}
