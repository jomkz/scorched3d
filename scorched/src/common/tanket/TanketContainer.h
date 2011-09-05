////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#if !defined(AFX_TanketContainer_H__56AF98E2_E188_45EC_AA25_1865ADBBA3F1__INCLUDED_)
#define AFX_TanketContainer_H__56AF98E2_E188_45EC_AA25_1865ADBBA3F1__INCLUDED_

#include <map>
#include <tanket/Tanket.h>
#include <target/TargetContainer.h>

class TanketContainer  
{
public:
	TanketContainer(TargetContainer &targets);
	virtual ~TanketContainer();

	void addTanket(Tanket *tanket);

	Tanket *removeTanket(unsigned int playerId);
	Tanket *getTanketById(unsigned int id);
		
	std::map<unsigned int, Tanket *> &getAllTankets(); // All tankets

protected:
	TargetContainer &targets_;
	std::map<unsigned int, Tanket *> tankets_;
};

#endif // !defined(AFX_TanketContainer_H__56AF98E2_E188_45EC_AA25_1865ADBBA3F1__INCLUDED_)
