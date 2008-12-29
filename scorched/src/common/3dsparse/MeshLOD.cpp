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


// MeshLOD.cpp: implementation of the MeshLOD class.
//
//////////////////////////////////////////////////////////////////////

#include <3dsparse/MeshLOD.h>
#include <3dsparse/MeshLODVector.h>
#include <3dsparse/MeshLODTri.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace MeshLOD
{
/* Start Namespace MeshLOD */

void addVertices(std::vector<MeshLODVector *> &vertices,
				 std::vector<Vertex *> &verts)
{
	for(int i=0; i<(int) verts.size(); i++) 
	{
		MeshLODVector *v = new MeshLODVector(verts[i]->position, i);
		vertices.push_back(v);
	}
}

void addTriangles(std::vector<MeshLODVector *> &vertices,
				  std::vector<MeshLODTri*> &triangles,
				  std::vector<Face *> &tri)
{
	for(int i=0; i<(int) tri.size(); i++) 
	{
		MeshLODTri *t=new MeshLODTri(
					      vertices[tri[i]->v[0]],
					      vertices[tri[i]->v[1]],
					      vertices[tri[i]->v[2]] );
		triangles.push_back(t);
	}
}

MeshLODVector *minimumCostEdge(std::vector<MeshLODVector *> &vertices)
{
	// Find the edge that when collapsed will affect model the least.
	// This funtion actually returns a Vertex, the second vertex
	// of the edge (collapse candidate) is stored in the vertex data.
	// Serious optimization opportunity here: this function currently
	// does a sequential search through an unsorted list :-(
	// Our algorithm could be O(n*lg(n)) instead of O(n*n)
	MeshLODVector *mn= vertices.front();

	std::vector<MeshLODVector *>::iterator itor;
	for (itor = vertices.begin();
		itor != vertices.end();
		itor++)
	{
		if((*itor)->objdist < mn->objdist) 
		{
			mn = *itor;
		}
	}

	return mn;
}

void computeAllEdgeCollapseCosts(std::vector<MeshLODVector *> &vertices)
{
	// For all the edges, compute the difference it would make
	// to the model if it was collapsed.  The least of these
	// per vertex is cached in each vertex object.
	std::vector<MeshLODVector *>::iterator itor;
	for (itor = vertices.begin();
		itor != vertices.end();
		itor++)
	{
		(*itor)->computeEdgeCostAtVertex();
	}
}

void removeVertex(std::vector<MeshLODVector *> &vertices, MeshLODVector *mn)
{
	std::vector<MeshLODVector *>::iterator itor;
	for (itor = vertices.begin();
		itor != vertices.end();
		itor++)
	{
		if (*itor == mn)
		{
			vertices.erase(itor);
			return;
		}
	}
}

void progressiveMesh(std::vector<Vertex *> &verts,
					 std::vector<Face *> &tri,
					 std::vector<int> &map)
{
	std::vector<MeshLODVector *> vertices;
	std::vector<MeshLODTri*> triangles;

	addVertices(vertices, verts);
	addTriangles(vertices, triangles, tri);

	// cache all edge collapse costs
	computeAllEdgeCollapseCosts(vertices); 

	// reduce the object down to nothing:
	map.resize(vertices.size(), 0);
	std::vector<int> permutation;
	permutation.resize(vertices.size(), 0);
	while(!vertices.empty()) 
	{
		// get the next vertex to collapse
		MeshLODVector *mn = minimumCostEdge(vertices);
		// keep track of this vertex, i.e. the collapse ordering
		permutation[mn->id] = ((int) vertices.size()) - 1;
		// keep track of vertex to which we collapse to
		map[vertices.size() - 1] = (mn->collapse)?mn->collapse->id:-1;
		// Collapse this edge
		mn->collapseVertex();

		removeVertex(vertices, mn);
		delete mn;
	}
	triangles.clear();

	// reorder the map list based on the collapse ordering
	unsigned int i;
	for(i=0;i<map.size();i++) 
	{
		map[i] = (map[i]==-1)?0:permutation[map[i]];
	}

	// Reorder the original vertex list
	std::vector<Vertex *> tmpVerts;
	for(i=0;i<verts.size();i++) 
	{
		tmpVerts.push_back(verts[i]);
	}
	for(i=0; i<verts.size(); i++) 
	{
		int j = permutation[i];
		verts[j] = tmpVerts[i];
	}

	// Reindex the face list
	for (i=0; i<tri.size(); i++)
	{
		for (int j=0; j<3; j++)
		{
			tri[i]->v[j] = permutation[tri[i]->v[j]];
		}
	}
}

/* End Namespace MeshLOD */
}
