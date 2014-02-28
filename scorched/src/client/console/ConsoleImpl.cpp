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

#include <console/ConsoleImpl.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <limits.h>

ConsoleImpl::ConsoleImpl() : 
	height_(0.0f), opening_(false), 
	lines_(1000), historyPosition_(-1), 
	showCursor_(true)
{
	Logger::addLogger(this, false);
}

ConsoleImpl::~ConsoleImpl()
{

}

void ConsoleImpl::init()
{
	methods_.init();
}

void ConsoleImpl::logMessage(LoggerInfo &info)
{
	addLine(false, info.getMessage());
}

void ConsoleImpl::resetPositions()
{
	historyPosition_ = -1;
	lines_.resetScroll();
}

void ConsoleImpl::addLine(bool parse, const std::string &text)
{
	lines_.addLine(text, parse);
	if (parse)
	{
		rules_.addLine(this, text.c_str());
	}
}

void ConsoleImpl::help()
{
	std::vector<std::string>::iterator itor;
	std::vector<std::string> result;
	rules_.dump(result);

	for (itor = result.begin();
		itor != result.end();
		++itor)
	{
		addLine(false, *itor);
	}
}
