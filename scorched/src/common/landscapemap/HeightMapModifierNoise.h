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

#if !defined(AFX_HEIGHTMAPMODIFIERNOISE_H__26733049_91D4_4AB9_8870_1BC94F531E9A__INCLUDED_)
#define AFX_HEIGHTMAPMODIFIERNOISE_H__26733049_91D4_4AB9_8870_1BC94F531E9A__INCLUDED_

#include <common/RandomGenerator.h>
#include <common/FixedVector.h>
#include <landscapemap/HeightMap.h>

class LandscapeDefnHeightMapGenerateNoise;

namespace HeightMapModifierNoise 
{
	void generateTerrain(HeightMap &hmap, 
		LandscapeDefnHeightMapGenerateNoise &defn,
		RandomGenerator &generator, 
		RandomGenerator &offsetGenerator,
		ProgressCounter *counter = 0);
};

#endif // !defined(AFX_HEIGHTMAPMODIFIERNOISE_H__26733049_91D4_4AB9_8870_1BC94F531E9A__INCLUDED_)
