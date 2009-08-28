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

#if !defined(AFX_RollingAverage_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_)
#define AFX_RollingAverage_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_

#include <common/fixed.h>

class RollingAverage  
{
public:
	RollingAverage(int numberAverages, fixed startValue);
	~RollingAverage();

	void addValue(fixed value);
	fixed getAverage() { return average_; }

private:
	int index_;
	int numberAverages_;
	fixed *averages_;
	fixed average_;
	fixed total_;
};

#endif // !defined(AFX_RollingAverage_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_)
