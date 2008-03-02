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

#if !defined(AFX_SurroundDefault_H__2FD73942_AF29_49ED_8DD8_925E2E14E464__INCLUDED_)
#define AFX_SurroundDefault_H__2FD73942_AF29_49ED_8DD8_925E2E14E464__INCLUDED_

#include <landscapemap/HeightMap.h>

class SurroundDefault  
{
public:
	SurroundDefault(HeightMap &map);
	virtual ~SurroundDefault();

	void draw(bool detail, bool lightMap);
	void generate();

protected:
	unsigned int listNo_;
	HeightMap &map_;
	Vector hMapBoxVerts_[16];
	void generateList(bool detail, bool lightMap);
	void generateVerts();

};

#endif // !defined(AFX_SurroundDefault_H__2FD73942_AF29_49ED_8DD8_925E2E14E464__INCLUDED_)
