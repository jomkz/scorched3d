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

#if !defined(__INCLUDE_UITankShotHistoryh_INCLUDE__)
#define __INCLUDE_UITankShotHistoryh_INCLUDE__

#include <net/NetBuffer.h>
#include <vector>

class UITankRenderer;
class UITankShotHistory
{
public:
	struct ShotEntry
	{
		ShotEntry(fixed p = 0, fixed r = 0, fixed e = 0) : 
			power(p), rot(r), ele(e) { }
	
		fixed power;
		fixed rot;
		fixed ele;
		bool current;
	};

	UITankShotHistory();
	virtual ~UITankShotHistory();

	ShotEntry &getCurrentValues() { return currentValues_; }

	// State change
	void clientNewGame();
	void madeShot();
	
	// Set
	fixed rotateGunXY(fixed angle, bool diff=true);
	fixed rotateGunYZ(fixed angle, bool diff=true);
	fixed changePower(fixed power, bool diff=true);
	void setMaxPower(fixed maxPower);

	// Saved settings
	fixed getRotationXYDiff();
	fixed getRotationYZDiff();
	fixed getPowerDiff();
	void revertSettings(unsigned int index = 0);
	void undo();
	std::vector<ShotEntry> &getOldShots();

	std::string getRotationString();
	std::string getElevationString();
	std::string getPowerString();

	void setTankRenderer(UITankRenderer *tankRenderer) { tankRenderer_ = tankRenderer; }

protected:
	UITankRenderer *tankRenderer_;
	ShotEntry oldValues_, currentValues_;
	std::vector<ShotEntry> oldShots_;
	fixed maxPower_;

	void settingsChanged();
};

#endif

