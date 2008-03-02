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

#if !defined(__INCLUDE_HeightMapSenderh_INCLUDE__)
#define __INCLUDE_HeightMapSenderh_INCLUDE__

#include <landscapemap/HeightMap.h>
#include <coms/ComsHeightMapMessage.h>
#include <common/ProgressCounter.h>

namespace HeightMapSender
{
	// Generate a heightmap diff that can be sent to a client
	// Store it in the coms message ready to be sent
	bool generateHMapDiff(
		HeightMap &hMap, 
		ComsHeightMapMessage &message,
		ProgressCounter *counter = 0);

	// Take a height map diff that has been recieved and apply
	// it to an already constructed heightmap
	bool generateHMapFromDiff(
		HeightMap &hMap, 
		ComsHeightMapMessage &message,
		ProgressCounter *counter = 0);
};

#endif // __INCLUDE_HeightMapSenderh_INCLUDE__
