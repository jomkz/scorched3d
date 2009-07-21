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

#include <actions/SkyFlash.h>
#include <engine/ScorchedContext.h>
#ifndef S3D_SERVER
	#include <landscape/Landscape.h>
	#include <sky/Sky.h>
#endif

SkyFlash::SkyFlash() :
	Action(0)
{
}

SkyFlash::~SkyFlash()
{
}

void SkyFlash::init()
{
}

void SkyFlash::simulate(fixed frameTime, bool &remove)
{
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		Landscape::instance()->getSky().flashSky();
	}
#endif // #ifndef S3D_SERVER

	remove = true;
	Action::simulate(frameTime, remove);
}
