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

#if !defined(__INCLUDE_ShotStateh_INCLUDE__)
#define __INCLUDE_ShotStateh_INCLUDE__

#include <engine/ScorchedContext.h>
#include <engine/PlayShots.h>

// Sends out the new game message
class ShotState
{
public:
	ShotState(ScorchedContext &context,
		PlayShots &playShots);
	virtual ~ShotState();

	void setup();
	bool run(float frameTime);

protected:
	ScorchedContext &context_;
	PlayShots &playShots_;
	bool firstTime_, lastTime_;

	void resurectTanks();

};

#endif