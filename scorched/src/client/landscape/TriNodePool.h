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


// TriNodePool.h: interface for the TriNodePool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRINODEPOOL_H__0A6192D7_293E_4695_AD2C_79372C0BB387__INCLUDED_)
#define AFX_TRINODEPOOL_H__0A6192D7_293E_4695_AD2C_79372C0BB387__INCLUDED_


#include <landscape/TriNode.h>

class TriNodePool  
{
public:
	static TriNodePool *instance();

    TriNode *getNextTriNode();
	void reset();

protected:
	static TriNodePool *instance_;
	TriNode *triNodeCount_;
	TriNode *triNodeMaxCount_;
	TriNode *nodes_;

private:
	TriNodePool();
	virtual ~TriNodePool();

};

#endif // !defined(AFX_TRINODEPOOL_H__0A6192D7_293E_4695_AD2C_79372C0BB387__INCLUDED_)
