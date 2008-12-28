////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_TargetMovementEntryh_INCLUDE__)
#define __INCLUDE_TargetMovementEntryh_INCLUDE__

#include <common/fixed.h>

class NetBuffer;
class NetBufferReader;
class RandomGenerator;
class ScorchedContext;
class LandscapeMovementType;
class TargetMovementEntry
{
public:
	virtual void generate(ScorchedContext &context, 
		RandomGenerator &random, 
		LandscapeMovementType *movementType) = 0;
	virtual void simulate(ScorchedContext &context, fixed frameTime) = 0;
	virtual void draw() = 0;

	virtual bool writeMessage(NetBuffer &buffer) = 0;
	virtual bool readMessage(NetBufferReader &reader) = 0;
};

#endif // __INCLUDE_TargetMovementEntryh_INCLUDE__
