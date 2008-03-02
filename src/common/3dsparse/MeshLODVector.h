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

#if !defined(AFX_MESHLODVECTOR_H__D2D4334B_59D8_42E3_A4CD_E6605D748F79__INCLUDED_)
#define AFX_MESHLODVECTOR_H__D2D4334B_59D8_42E3_A4CD_E6605D748F79__INCLUDED_

#include <list>
#include <common/Vector.h>

class MeshLODTri;
class MeshLODVector : public Vector
{
public:
	MeshLODVector(Vector &v, int _id);
	virtual ~MeshLODVector();

	MeshLODVector	*collapse; // candidate vertex for collapse

	int              id;       // place of vertex in original list
	float            objdist;  // cached cost of collapsing edge

	std::list<MeshLODVector *>   neighbor; // adjacent vertices
	std::list<MeshLODTri *> face;     // adjacent triangles
	
	void            removeIfNonNeighbor(MeshLODVector *n);
	void			addNeighbour(MeshLODVector *vector);
	void			removeNeighbor(MeshLODVector *vector);
	void			removeFace(MeshLODTri *face);

	float			computeEdgeCollapseCost(MeshLODVector *neigbour);
	void			computeEdgeCostAtVertex();
	void			collapseVertex();
};

#endif // !defined(AFX_MESHLODVECTOR_H__D2D4334B_59D8_42E3_A4CD_E6605D748F79__INCLUDED_)
