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


#if !defined(AFX_MESHLODTRI_H__0EB05DEC_5B60_4C5A_BD4A_372E237D2140__INCLUDED_)
#define AFX_MESHLODTRI_H__0EB05DEC_5B60_4C5A_BD4A_372E237D2140__INCLUDED_

#include <common/Vector.h>

class MeshLODVector;

class MeshLODTri  
{
public:
	MeshLODTri(MeshLODVector *v0,MeshLODVector *v1,MeshLODVector *v2);
	virtual ~MeshLODTri();

	MeshLODVector *  vertex[3]; // the 3 points that make this tri
	Vector           normal;    // unit vector othogonal to this face
	                	                 
	void             computeNormal();
	void             replaceVertex(MeshLODVector *vold, MeshLODVector *vnew);
	bool             hasVertex(MeshLODVector *v);
};

#endif // !defined(AFX_MESHLODTRI_H__0EB05DEC_5B60_4C5A_BD4A_372E237D2140__INCLUDED_)
