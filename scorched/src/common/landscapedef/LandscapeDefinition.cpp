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

#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

LandscapeDefinition::LandscapeDefinition(
	const char *tex,
	const char *defn,
	unsigned int seed,
	const char *name,
	unsigned int definitionNumber) :
	tex_(tex), defn_(defn), seed_(seed), name_(name),
	definitionNumber_(definitionNumber)
{
}

bool LandscapeDefinition::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(seed_);
	buffer.addToBuffer(tex_);
	buffer.addToBuffer(defn_);
	buffer.addToBuffer(name_);
	buffer.addToBuffer(definitionNumber_);
	return true;
}

bool LandscapeDefinition::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(seed_)) return false;
	if (!reader.getFromBuffer(tex_)) return false;
	if (!reader.getFromBuffer(defn_)) return false;
	if (!reader.getFromBuffer(name_)) return false;
	if (!reader.getFromBuffer(definitionNumber_)) return false;
	return true;
}
