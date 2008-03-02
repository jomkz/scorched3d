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


// MeshLODVector.cpp: implementation of the MeshLODVector class.
//
//////////////////////////////////////////////////////////////////////

#include <3dsparse/MeshLODTri.h>
#include <3dsparse/MeshLODVector.h>
#include <common/Defines.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MeshLODVector::MeshLODVector(Vector &v, int _id)
	: Vector(v), id(_id)
{

}

MeshLODVector::~MeshLODVector()
{

}

void MeshLODVector::addNeighbour(MeshLODVector *vector)
{
	if (vector != this)
	{
		bool found = false;
		std::list<MeshLODVector *>::iterator neighborItor;
		for (neighborItor = neighbor.begin();
			neighborItor != neighbor.end();
			neighborItor++)
		{
			if (*neighborItor == vector) 
			{
				found = true;
				break;
			}
		}
		if (!found) neighbor.push_back(vector);
	}
}

void MeshLODVector::removeNeighbor(MeshLODVector *vector)
{
	for (unsigned int i=0; i<neighbor.size(); i++)
	{
		MeshLODVector *vec = neighbor.front();
		neighbor.pop_front();
		if (vec == vector) 
		{
			break;
		}
		neighbor.push_back(vec);
	}
}

void MeshLODVector::removeIfNonNeighbor(MeshLODVector *n) 
{
	// removes n from neighbor list if n isn't a neighbor.
	std::list<MeshLODTri *>::iterator faceItor;
	for (faceItor = face.begin();
		faceItor != face.end();
		faceItor++)
	{
		MeshLODTri *tri = (*faceItor);
		if(tri->hasVertex(n)) return;
	}
	removeNeighbor(n);
}

void MeshLODVector::removeFace(MeshLODTri *rface)
{
	for (unsigned int i=0; i<face.size(); i++)
	{
		MeshLODTri *tri = face.front();
		face.pop_front();
		if (tri == rface)
		{
			break;
		}
		face.push_back(tri);
	}
}

float MeshLODVector::computeEdgeCollapseCost(MeshLODVector *neigbour)
{
	//(Vertex *u,Vertex *v)
	// if we collapse edge uv by moving u to v then how 
	// much different will the model change, i.e. how much "error".
	// Texture, vertex normal, and border vertex code was removed
	// to keep this demo as simple as possible.
	// The method of determining cost was designed in order 
	// to exploit small and coplanar regions for
	// effective polygon reduction.
	// Is is possible to add some checks here to see if "folds"
	// would be generated.  i.e. normal of a remaining face gets
	// flipped.  I never seemed to run into this problem and
	// therefore never added code to detect this case.

	float edgelength = (*neigbour - *this).Magnitude();

	// find the "sides" triangles that are on the edge uv
	std::list<MeshLODTri *> sides;
	std::list<MeshLODTri *>::iterator faceItor;
	for (faceItor = face.begin();
		faceItor != face.end();
		faceItor++)
	{
		if ((*faceItor)->hasVertex(neigbour))
		{
			sides.push_back(*faceItor);
		}
	}

	// use the triangle facing most away from the sides 
	// to determine our curvature term
	float curvature = 0;
	for (faceItor = face.begin();
		faceItor != face.end();
		faceItor++)
	{
		float mincurv = 1; // curve for face i and closer side to it

		std::list<MeshLODTri *>::iterator sideItor;
		for (sideItor = sides.begin();
			sideItor != sides.end();
			sideItor++)
		{
			// use dot product of face normals. '^' defined in vector
			float dotprod = (*faceItor)->normal.dotP((*sideItor)->normal);
			mincurv = MIN(mincurv,(1-dotprod)/2.0f);
		}

		curvature = MAX(curvature,mincurv);
	}

	// the more coplanar the lower the curvature term   
	return edgelength * curvature;
}

void MeshLODVector::computeEdgeCostAtVertex()
{
	// compute the edge collapse cost for all edges that start
	// from vertex v.  Since we are only interested in reducing
	// the object by selecting the min cost edge at each step, we
	// only cache the cost of the least cost edge at this vertex
	// (in member variable collapse) as well as the value of the 
	// cost (in member variable objdist).

	collapse = NULL;
	if(neighbor.empty()) 
	{
		// v doesn't have neighbors so it costs nothing to collapse
		objdist = -0.01f;
	}
	else
	{
		// search all neighboring edges for "least cost" edge
		objdist = 1000000;
		std::list<MeshLODVector *>::iterator neighborItor;
		for (neighborItor = neighbor.begin();
			neighborItor != neighbor.end();
			neighborItor++)
		{
			float dist = computeEdgeCollapseCost(*neighborItor);
			if(dist < objdist) 
			{
				collapse = *neighborItor;   // candidate for edge collapse
				objdist = dist;             // cost of the collapse
			}
		}
	}
}

void MeshLODVector::collapseVertex()

{
	// Vertex *u = this,Vertex *v = collapse
	// Collapse the edge uv by moving vertex u onto v
	// Actually remove tris on uv, then update tris that
	// have u to have v, and then remove u.

	if(!collapse) 
	{
		// u is a vertex all by itself so just delete it
	}
	else
	{
		// make tmp a list of all the neighbors of u
		std::list<MeshLODVector *> tmp(neighbor);

		// delete triangles on edge uv:
		std::list<MeshLODTri *> delList(face);
		while (!delList.empty())
		{
			MeshLODTri *tri = delList.front();
			if (tri->hasVertex(collapse))
			{
				delete tri;
			}
			delList.pop_front();
		}

		// update remaining triangles to have v instead of u
		std::list<MeshLODTri *> tmpList(face);
		while (!tmpList.empty())
		{
			MeshLODTri *tri = tmpList.front();
			tmpList.pop_front();
			tri->replaceVertex(this, collapse);
		}

		// Remove from neigbours lists
		while(!neighbor.empty()) 
		{
			neighbor.back()->removeNeighbor(this);
			neighbor.pop_back();
		}

		// recompute the edge collapse costs for neighboring vertices
		std::list<MeshLODVector *>::iterator tmpItor = tmp.begin();
		for (tmpItor = tmp.begin();
			tmpItor != tmp.end();
			tmpItor++)
		{
			(*tmpItor)->computeEdgeCostAtVertex();
		}
	}
}
