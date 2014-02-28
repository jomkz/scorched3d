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
#include <common/Clock.h>

Clock::Clock() 
{
	reset();
}

Clock::~Clock()
{
}

void Clock::reset()
{
	lastTime_ = boost::posix_time::microsec_clock::local_time();
}

unsigned int Clock::getTicksDifference()
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration diff = now - lastTime_;

	unsigned int timeDiff = (unsigned int) diff.total_milliseconds();
	if (timeDiff == 0) timeDiff = 1;

	lastTime_ = now;
	return timeDiff;
}

float Clock::getTimeDifference()
{
	unsigned int  dwTimeDiff = getTicksDifference();
	return ((float) dwTimeDiff) / 1000.0f;
}

