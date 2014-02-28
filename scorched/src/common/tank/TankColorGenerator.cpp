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

#include <tank/TankColorGenerator.h>
#include <tank/Tank.h>
#include <stdlib.h>

TankColorGenerator *TankColorGenerator::instance_ = 0;

TankColorGenerator *TankColorGenerator::instance()
{
	if (!instance_)
	{
		instance_ = new TankColorGenerator;
	}
	return instance_;
}

TankColorGenerator::TankColorGenerator()
{
	addColor(255, 0, 0);
	addColor(255, 255, 0);
	addColor(255, 120, 0);
	addColor(140, 0, 255);
	addColor(255, 0, 255);
	addColor(0, 0, 255);
	addColor(0, 255, 0);
	addColor(255, 190, 0);
	addColor(255, 255, 140);
	addColor(140, 255, 255);
	addColor(140, 140, 255);
	addColor(140, 0, 0);
	addColor(140, 140, 0);
	addColor(140, 140, 140);
	addColor(140, 255, 0);
	addColor(255, 0, 140);
	addColor(140, 255, 140);	
	addColor(0, 0, 140);
	addColor(0, 140, 0);
	addColor(0, 255, 140);
	addColor(140, 0, 140);
	addColor(0, 255, 255);
	addColor(0, 140, 255);
	addColor(0, 140, 140);
	addColor(255, 140, 255);
	addColor(190, 125, 35);
}

TankColorGenerator::~TankColorGenerator()
{
}

void TankColorGenerator::addColor(unsigned r, unsigned g, unsigned b)
{
	FixedVector newColor(
		fixed(true, r) / fixed(255),
		fixed(true, g) / fixed(255),
		fixed(true, b) / fixed(255));
	availableColors_.push_back(new FixedVector(newColor));
}

bool TankColorGenerator::colorAvailable(FixedVector &color,
	std::map<unsigned int, Tank *> &tanks,
	Tank *currentTank)
{
	bool available = true;
	std::map<unsigned int, Tank *>::iterator tankitor;
	for (tankitor = tanks.begin();
		tankitor != tanks.end();
		++tankitor)
	{
		Tank *tank = (*tankitor).second;
		if (tank->getColor() == color)
		{
			if (currentTank != tank)
			{
				available = false;
				break;
			}
		}
	}

	return available;
}

std::vector<FixedVector *> TankColorGenerator::getAvailableColors(
	std::map<unsigned int, Tank *> &tanks,
	Tank *currentTank)
{
	std::vector<FixedVector*> leftColors;
	std::vector<FixedVector*>::iterator coloritor;
	for (coloritor = availableColors_.begin();
		coloritor != availableColors_.end();
		++coloritor)
	{
		FixedVector *color = (*coloritor);
		if (colorAvailable(*color, tanks, currentTank))
		{
			leftColors.push_back(color);
		}
	}
	return leftColors;
}

FixedVector &TankColorGenerator::getNextColor(std::map<unsigned int, Tank *> &tanks)
{
	std::vector<FixedVector *> leftColors = getAvailableColors(tanks);
	if (!leftColors.empty())
	{
		return *leftColors[0];
	}

	static FixedVector defaultColor(fixed(true, 8000), fixed(true, 8000), fixed(true, 8000));
	return defaultColor;
}

FixedVector &TankColorGenerator::getTeamColor(int team)
{
	static FixedVector red(1, 0, 0);
	static FixedVector blue(0, fixed(true, 3000), 1);
	static FixedVector green(0, 1, 0);
	static FixedVector yellow(1, 1, 0);
	if (team == 1) return red;
	else if (team == 2) return blue;
	else if (team == 3) return green;
	else if (team == 4) return yellow;
	return FixedVector::getNullVector();
}

const char *TankColorGenerator::getTeamName(int team)
{
	if (team == 1) return "Red";
	else if (team == 2) return "Blue";
	else if (team == 3) return "Green";
	else if (team == 4) return "Yellow";
	return "None";
}
