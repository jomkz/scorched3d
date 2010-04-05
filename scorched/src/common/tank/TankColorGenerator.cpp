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
	addColor(0x7F, 0xFF, 0xD4);
	addColor(0xD2, 0x69, 0x1E);
	addColor(0x8B, 0x00, 0x8B);
	addColor(0x99, 0x32, 0xCC);
	addColor(0x8D, 0xBC, 0x8F);
	addColor(0x00, 0xDE, 0xD1);
	addColor(0xFF, 0x8C, 0x00);
	addColor(0xFF, 0xE4, 0xC4);
	addColor(0x00, 0xBF, 0xFF);
	addColor(0xB2, 0x22, 0x22);
	addColor(0xFF, 0x00, 0xFF);
	addColor(0xFF, 0xD7, 0x00);
	addColor(0xAD, 0xD8, 0xE6);
	addColor(0xFF, 0xB6, 0xC1);
	addColor(0xFA, 0xFA, 0xD2);
	addColor(0x00, 0x22, 0xFF);
	addColor(0xDE, 0xB8, 0x87);
	addColor(0xFF, 0x45, 0x00);
	addColor(0xDA, 0xA5, 0x20);
	addColor(0xFF, 0xFF, 0x20);
	addColor(0xFF, 0x00, 0x20);
	addColor(0x2E, 0x8B, 0x57);
	addColor(0xBD, 0xB7, 0x6B);
	addColor(0x7F, 0xFF, 0x00);
}

TankColorGenerator::~TankColorGenerator()
{
}

void TankColorGenerator::addColor(unsigned r, unsigned g, unsigned b)
{
	Vector newColor(
		float(int(float(r) / 2.550f)) / 100.0f, 
		float(int(float(g) / 2.550f)) / 100.0f, 
		float(int(float(b) / 2.550f)) / 100.0f);
	
	std::vector<Vector*>::iterator coloritor;
	for (coloritor = availableColors_.begin();
		coloritor != availableColors_.end();
		coloritor++)
	{
		Vector &color = *(*coloritor);
		if (newColor[0] > color[0]) break;
		else if (newColor[0] == color[0])
		{
			if (newColor[1] > color[1]) break;
			else if (newColor[1] == color[1])
			{
				if (newColor[2] > color[2]) break;
				else if (newColor[2] == color[2])
				{
					break;
				}
			}
		}
	}

	availableColors_.insert(coloritor, new Vector(newColor));
}

bool TankColorGenerator::colorAvailable(Vector &color,
	std::map<unsigned int, Tank *> &tanks,
	Tank *currentTank)
{
	bool available = true;
	std::map<unsigned int, Tank *>::iterator tankitor;
	for (tankitor = tanks.begin();
		tankitor != tanks.end();
		tankitor++)
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

std::vector<Vector *> TankColorGenerator::getAvailableColors(
	std::map<unsigned int, Tank *> &tanks,
	Tank *currentTank)
{
	std::vector<Vector*> leftColors;
	std::vector<Vector*>::iterator coloritor;
	for (coloritor = availableColors_.begin();
		coloritor != availableColors_.end();
		coloritor++)
	{
		Vector *color = (*coloritor);
		if (colorAvailable(*color, tanks, currentTank))
		{
			leftColors.push_back(color);
		}
	}
	return leftColors;
}

Vector &TankColorGenerator::getNextColor(std::map<unsigned int, Tank *> &tanks)
{
	std::vector<Vector *> leftColors = getAvailableColors(tanks);
	if (!leftColors.empty())
	{
		return *leftColors[0];
	}

	static Vector defaultColor(0.8f, 0.8f, 0.8f);
	return defaultColor;
}

Vector &TankColorGenerator::getTeamColor(int team)
{
	static Vector red(1.0f, 0.0f, 0.0f);
	static Vector blue(0.0f, 0.3f, 1.0f);
	static Vector green(0.0f, 1.0f, 0.0f);
	static Vector yellow(1.0f, 1.0f, 0.0f);
	if (team == 1) return red;
	else if (team == 2) return blue;
	else if (team == 3) return green;
	else if (team == 4) return yellow;
	return Vector::getNullVector();
}

const char *TankColorGenerator::getTeamName(int team)
{
	if (team == 1) return "Red";
	else if (team == 2) return "Blue";
	else if (team == 3) return "Green";
	else if (team == 4) return "Yellow";
	return "None";
}
