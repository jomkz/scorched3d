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


// MeshLODTri.cpp: implementation of the MeshLODTri class.
//
//////////////////////////////////////////////////////////////////////

#include <common/Defines.h>
#include <3dsparse/MeshLODVector.h>
#include <3dsparse/MeshLODTri.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MeshLODTri::MeshLODTri(MeshLODVector *v0,MeshLODVector *v1,MeshLODVector *v2)
{
	vertex[0]=v0;
	vertex[1]=v1;
	vertex[2]=v2;

	//DIALOG_ASSERT(vertex[0]!=vertex[1] && vertex[0]!=vertex[2] && vertex[1]!=vertex[2]);

	for(int i=0;i<3;i++) 
	{
		vertex[i]->face.push_back(this);
		for(int j=0;j<3;j++)
		{
			vertex[i]->addNeighbour(vertex[j]);
		}
	}

	computeNormal();
}

MeshLODTri::~MeshLODTri()
{
	int i;
	for(i=0;i<3;i++) 
	{
		vertex[i]->removeFace(this);
	}

	for(i=0;i<3;i++) 
	{
		int i2 = (i+1)%3;
		vertex[i ]->removeIfNonNeighbor(vertex[i2]);
		vertex[i2]->removeIfNonNeighbor(vertex[i ]);
	}

	for (i=0;i<3;i++)
	{
		vertex[i] = 0;
	}
}

void MeshLODTri::computeNormal()
{
	normal = ((*vertex[1] - *vertex[0]) * (*vertex[2] - *vertex[1])).Normalize();
}

bool MeshLODTri::hasVertex(MeshLODVector *v)
{
	return (v==vertex[0] || v==vertex[1] || v==vertex[2]);
}

void MeshLODTri::replaceVertex(MeshLODVector *vold, MeshLODVector *vnew) 
{
	DIALOG_ASSERT(vold && vnew);
	DIALOG_ASSERT(vold==vertex[0] || vold==vertex[1] || vold==vertex[2]);
	//DIALOG_ASSERT(vnew!=vertex[0] && vnew!=vertex[1] && vnew!=vertex[2]);

	if(vold==vertex[0])
	{
		vertex[0]=vnew;
	}
	else if(vold==vertex[1])
	{
		vertex[1]=vnew;
	}
	else 
	{
		DIALOG_ASSERT(vold==vertex[2]);
		vertex[2]=vnew;
	}

	vold->removeFace(this);
	vnew->face.push_back(this);

	int i;
	for(i=0;i<3;i++) 
	{
		vold->removeIfNonNeighbor(vertex[i]);
		vertex[i]->removeIfNonNeighbor(vold);
	}

	for(i=0;i<3;i++) 
	{
		for(int j=0;j<3;j++) 
		{
			vertex[i]->addNeighbour(vertex[j]);
		}
	}

	computeNormal();
}
