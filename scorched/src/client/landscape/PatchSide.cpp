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

#include <math.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLDynamicVertexArray.h>
#include <GLEXT/GLStateExtension.h>
#include <landscape/PatchSide.h>

static Vector leftN;
static Vector rightN;
static Vector apexN;

PatchSide::PatchSide(HeightMap *hMap, PatchTexCoord *coord, 
	int left, int top, int width) : 
	triNodePool_(*TriNodePool::instance()),
	hMap_(hMap), coord_(coord),
	variance_(hMap, left, top, width),
	left_(left), top_(top), width_(width)
{

}

PatchSide::~PatchSide()
{

}

TriNode *PatchSide::getTriNode()
{
	return &baseTriNode_;
}

void PatchSide::reset()
{
	baseTriNode_.LeftChild = 0;
	baseTriNode_.RightChild = 0;
	baseTriNode_.LeftNeighbor = 0;
	baseTriNode_.RightNeighbor = 0;
}

void PatchSide::computeVariance()
{
	variance_.computeVariance();
}

void PatchSide::split(TriNode *tri)
{
	// We are already split, no need to do it again.
	if (tri->LeftChild)
		return;

	// If this triangle is not in a proper diamond, force split our base neighbor
	if ( tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor != tri) )
	{
		split(tri->BaseNeighbor);
	}

	// Create children and link into mesh
	TriNode *cl = triNodePool_.getNextTriNode();
	TriNode *cr = triNodePool_.getNextTriNode();

	// If creation failed, just exit.
	if ( !cl || !cr) return;

	tri->RightChild = cr;
	tri->LeftChild  = cl;

	// Fill in the information we can get from the parent (neighbor pointers)
	tri->LeftChild->BaseNeighbor  = tri->LeftNeighbor;
	tri->LeftChild->LeftNeighbor  = tri->RightChild;

	tri->RightChild->BaseNeighbor  = tri->RightNeighbor;
	tri->RightChild->RightNeighbor = tri->LeftChild;

	// Link our Left Neighbor to the new children
	if (tri->LeftNeighbor != NULL)
	{
		if (tri->LeftNeighbor->BaseNeighbor == tri)
			tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->LeftNeighbor == tri)
			tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->RightNeighbor == tri)
			tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
		else
			;// Illegal Left Neighbor!
	}

	// Link our Right Neighbor to the new children
	if (tri->RightNeighbor != NULL)
	{
		if (tri->RightNeighbor->BaseNeighbor == tri)
			tri->RightNeighbor->BaseNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->RightNeighbor == tri)
			tri->RightNeighbor->RightNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->LeftNeighbor == tri)
			tri->RightNeighbor->LeftNeighbor = tri->RightChild;
		else
			;// Illegal Right Neighbor!
	}

	// Link our Base Neighbor to the new children
	if (tri->BaseNeighbor != NULL)
	{
		if ( tri->BaseNeighbor->LeftChild )
		{
			tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
			tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
			tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
			tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
		}
		else
		{
				split( tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
		}
	}
	else
	{
		// An edge triangle, trivial case.
		tri->LeftChild->RightNeighbor = NULL;
		tri->RightChild->LeftNeighbor = NULL;
	}
}

void PatchSide::recursTessellate( TriNode *tri,
							  int leftX,  int leftY,
							  int rightX, int rightY,
							  int apexX,  int apexY,
							  int node, unsigned currentVariance )
{
	int centerX = (leftX + rightX) >> 1; // Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY) >> 1; // Compute Y coord.

	unsigned int triVariance = 100000;
	if ((unsigned int) node < variance_.getMaxPos() ) // If we are not below the variance tree
	{	
		triVariance = variance_.getVariance(node);	// Take variance into consideration
	}
	else
	{
		triVariance = 1;
	}

	if (triVariance >= currentVariance)
	{
		split(tri);														// Split this triangle.

		if (tri->LeftChild &&											// If this triangle was split, try to split it's children as well.
			((abs(leftX - rightX) >= 2) || (abs(leftY - rightY) >= 2 )))	// Tessellate all the way down to one vertex per height field entry
		{
			recursTessellate( tri->LeftChild,   apexX,  apexY, leftX, leftY, centerX, centerY,    node<<1 , currentVariance);
			recursTessellate( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, 1+(node<<1), currentVariance);
		}
	}
}

void PatchSide::tesselate(unsigned currentVariance)
{
	recursTessellate(	&baseTriNode_,
						left_ + width_,		top_ + width_,
						left_,				top_,
						left_ + width_,				top_,
						1, currentVariance);
}

void PatchSide::recursRender( TriNode *tri, 
							int leftX, int leftY, 
							int rightX, int rightY, 
							int apexX, int apexY, 
							int node, DrawType side)
{
	if ( tri->LeftChild )
	{
		int centerX = (leftX + rightX) >> 1;
		int centerY = (leftY + rightY) >> 1;

		recursRender( tri->LeftChild, apexX,   apexY, leftX, leftY, centerX, centerY, node<<1, side);
		recursRender( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, 1+(node<<1), side);
	}
	else
	{
		GLfloat leftZ  = hMap_->getHeight(leftX, leftY).asFloat();
		hMap_->getNormal(leftX, leftY).asVector(leftN);
		GLfloat rightZ = hMap_->getHeight(rightX, rightY).asFloat();
		hMap_->getNormal(rightX, rightY).asVector(rightN);
		GLfloat apexZ  = hMap_->getHeight(apexX, apexY).asFloat();
		hMap_->getNormal(apexX, apexY).asVector(apexN);

		switch (side)
		{
		case typeTop:
			GLDynamicVertexArray::Data *data;

			// Left
			data = GLDynamicVertexArray::instance()->getData();
			data->x = (GLfloat) leftX;
			data->y = (GLfloat) leftY;
			data->z = (GLfloat) leftZ;
			data->t1x = coord_->getWidthEntry(leftX).txa;
			data->t1y = coord_->getHeightEntry(leftY).txa;
			data->t2x = coord_->getWidthEntry(leftX).txb;
			data->t2y = coord_->getHeightEntry(leftY).txb;
			data->nx = leftN[0];
			data->ny = leftN[1];
			data->nz = leftN[2];

			// Right
			data = GLDynamicVertexArray::instance()->getData();
			data->x = (GLfloat) rightX;
			data->y = (GLfloat) rightY;
			data->z = (GLfloat) rightZ;
			data->t1x = coord_->getWidthEntry(rightX).txa;
			data->t1y = coord_->getHeightEntry(rightY).txa;
			data->t2x = coord_->getWidthEntry(rightX).txb;
			data->t2y = coord_->getHeightEntry(rightY).txb;
			data->nx = rightN[0];
			data->ny = rightN[1];
			data->nz = rightN[2];

			// Apex
			data = GLDynamicVertexArray::instance()->getData();
			data->x = (GLfloat) apexX;
			data->y = (GLfloat) apexY;
			data->z = (GLfloat) apexZ;
			data->t1x = coord_->getWidthEntry(apexX).txa;
			data->t1y = coord_->getHeightEntry(apexY).txa;
			data->t2x = coord_->getWidthEntry(apexX).txb;
			data->t2y = coord_->getHeightEntry(apexY).txb;
			data->nx = apexN[0];
			data->ny = apexN[1];
			data->nz = apexN[2];

			if (GLDynamicVertexArray::instance()->getSpace() < 100)
			{
				GLDynamicVertexArray::instance()->drawROAM();
			}
			break;
		case typeNormals:
		{
			hMap_->getNormal(leftX, leftY).asVector(leftN);
			hMap_->getNormal(rightX, rightY).asVector(rightN);
			hMap_->getNormal(apexX, apexY).asVector(apexN);

			glVertex3f((GLfloat) leftX, (GLfloat) leftY, (GLfloat) leftZ );
			glVertex3f((GLfloat) leftX + leftN[0] * 5.0f, 
					(GLfloat) leftY + leftN[1] * 5.0f, 
					(GLfloat) leftZ + leftN[2] * 5.0f);

			glVertex3f((GLfloat) rightX,(GLfloat) rightY,(GLfloat) rightZ );
			glVertex3f((GLfloat) rightX + rightN[0] * 5.0f, 
					(GLfloat) rightY + rightN[1] * 5.0f, 
					(GLfloat) rightZ + rightN[2] * 5.0f);

			glVertex3f((GLfloat) apexX, (GLfloat) apexY, (GLfloat) apexZ );
			glVertex3f((GLfloat) apexX + apexN[0] * 5.0f, 
					(GLfloat) apexY + apexN[1] * 5.0f, 
					(GLfloat) apexZ + apexN[2] * 5.0f);
		}
		break;
		}
	}
}

void PatchSide::draw(DrawType side)
{
	recursRender (	&baseTriNode_,
			left_ + width_,		top_ + width_,
			left_,				top_,	
			left_ + width_,				top_,
			1, side);
}
