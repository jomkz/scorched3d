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

#include <3dsparse/TreeModelFactory.h>
#include <common/Defines.h>
#include <stdio.h>
#include <math.h>

TreeModelFactory::TreeModelFactory()
{
}

TreeModelFactory::~TreeModelFactory()
{
}

Model *TreeModelFactory::createModel(const char *fileName,
	const char *texName)
{
	Model *model = new Model();
	model->getMin() = Vector(-0.5f, -0.5f, -0.5f);
	model->getMax() = Vector(+0.5f, +0.5f, +0.5f);
	return model;
}

bool TreeModelFactory::getTypes(const char *type, bool snow, 
	TreeType &normalType, TreeType &burntType)
{
	if (0 == strcmp(type, "pine"))
	{
		normalType = (snow?ePineSnow:ePineNormal);
		burntType = ePineBurnt;
	}
	else if (0 == strcmp(type, "pine2"))
	{
		normalType = (snow?ePine2Snow:ePine2);
		burntType = ePineBurnt;
	}
	else if (0 == strcmp(type, "pine3"))
	{
		normalType = (snow?ePine3Snow:ePine3);;
		burntType = ePineBurnt;
	}
	else if (0 == strcmp(type, "pine4"))
	{
		normalType = (snow?ePine4Snow:ePine4);;
		burntType = ePineBurnt;
	}
	else if (0 == strcmp(type, "burntpine"))
	{
		normalType = ePineBurnt;
		burntType = ePineBurnt;
	}
	else if (0 == strcmp(type, "yellowpine"))
	{
		normalType = (snow?ePineSnow:ePineYellow);
		burntType = ePineBurnt;
	}
	else if (0 == strcmp(type, "lightpine"))
	{
		normalType = (snow?ePineSnow:ePineLight);
		burntType = ePineBurnt;
	}
	else if (0 == strcmp(type, "palm"))
	{
		normalType = ePalmNormal;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palm2"))
	{
		normalType = ePalm2;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palm3"))
	{
		normalType = ePalm3;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palm4"))
	{
		normalType = ePalm4;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palmb"))
	{
		normalType = ePalmB;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palmb2"))
	{
		normalType = ePalmB2;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palmb3"))
	{
		normalType = ePalmB3;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palmb4"))
	{
		normalType = ePalmB4;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palmb5"))
	{
		normalType = ePalmB5;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palmb6"))
	{
		normalType = ePalmB6;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "palmb7"))
	{
		normalType = ePalmB7;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "burntpalm"))
	{
		normalType = ePalmBurnt;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "oak"))
	{
		normalType = eOak;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "oak2"))
	{
		normalType = eOak2;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "oak3"))
	{
		normalType = eOak3;
		burntType = ePalmBurnt;
	}
	else if (0 == strcmp(type, "oak4"))
	{
		normalType = eOak4;
		burntType = ePalmBurnt;
	}
	else return false;
	return true;
}

