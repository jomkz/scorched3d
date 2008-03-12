////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include <landscape/Surround.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefn.h>
#include <land/VisibilityPatchGrid.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>

Surround::Surround(HeightMap &map) : 
	default_(map)
{
}

Surround::~Surround()
{
}

void Surround::draw(bool detail, bool lightMap)
{
	VisibilityPatchGrid::instance()->drawSurround();

	//default_.draw(detail, lightMap);
	if (ScorchedClient::instance()->getLandscapeMaps().getDefinitions().
		getDefn()->surround->getType() != LandscapeDefnType::eNone &&
		OptionsDisplay::instance()->getDrawSurround())
	{
		landscape_.draw(detail, lightMap);
	}
}

void Surround::generate()
{
	default_.generate();
	landscape_.generate();
}

