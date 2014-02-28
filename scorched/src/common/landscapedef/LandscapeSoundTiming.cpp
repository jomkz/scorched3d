////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <landscapedef/LandscapeSoundTiming.h>

LandscapeSoundTimingChoice::LandscapeSoundTimingChoice() :
	XMLEntryTypeChoice<LandscapeSoundTiming>("timing", 
		"Associates sounds with a given timing loop")
{
}

LandscapeSoundTimingChoice::~LandscapeSoundTimingChoice()
{
}

LandscapeSoundTiming *LandscapeSoundTimingChoice::createXMLEntry(const std::string &type)
{
	if (0 == strcmp(type.c_str(), "looped")) return new LandscapeSoundTimingLooped;
	if (0 == strcmp(type.c_str(), "repeat")) return new LandscapeSoundTimingRepeat;
	S3D::dialogMessage("LandscapeSoundTimingChoice", S3D::formatStringBuffer("Unknown sound type %s", type));
	return 0;
}

LandscapeSoundTiming::LandscapeSoundTiming(const char *name, const char *description) :
	XMLEntryContainer("LandscapeSoundTiming", "Specifies how often sounds will repeat")
{
}

LandscapeSoundTiming::~LandscapeSoundTiming()
{
}

LandscapeSoundTimingLooped::LandscapeSoundTimingLooped() :
	LandscapeSoundTiming("LandscapeSoundTimingLooped", 
		"Loops the sound continually with no pause")
{
}

LandscapeSoundTimingLooped::~LandscapeSoundTimingLooped()
{
}

float LandscapeSoundTimingLooped::getNextEventTime()
{
	return -1.0f;
}

LandscapeSoundTimingRepeat::LandscapeSoundTimingRepeat() :
	LandscapeSoundTiming("LandscapeSoundTimingRepeat", 
		"Loops the sound pausing for between min and max seconds"),
	min("min", "The minimum amount of time to wait before playing this sound again"),
	max("max", "The maximum amount of time to wait before playing this sound again")
{
}

LandscapeSoundTimingRepeat::~LandscapeSoundTimingRepeat()
{
}

float LandscapeSoundTimingRepeat::getNextEventTime()
{
	fixed result = min.getValue() + max.getValue() * fixed(true, rand() % fixed::FIXED_RESOLUTION);
	return result.asFloat();
}
