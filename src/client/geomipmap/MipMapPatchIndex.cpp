////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <geomipmap/MipMapPatchIndex.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLVertexBufferObject.h>
#include <GLEXT/GLStateExtension.h>
#include <vector>

MipMapPatchIndex::MipMapPatchIndex() : 
	indices_(0), size_(0), bufferObject_(0)
{
}

MipMapPatchIndex::~MipMapPatchIndex()
{
	delete [] indices_;
}

void MipMapPatchIndex::generate(int size, int totalsize, int skip, unsigned int border)
{
	// Calculate the border (if any)
	unsigned int borderLeft  = (border & BorderLeft)  >> 0;
	unsigned int borderRight = (border & BorderRight) >> 3;
	unsigned int borderTop   = (border & BorderTop)   >> 6;
	unsigned int borderBottom= (border & BorderBottom)>> 9;

	int borderLeftSkip = skip * (1 << borderLeft);
	int borderRightSkip = skip * (1 << borderRight);
	int borderTopSkip = skip * (1 << borderTop);
	int borderBottomSkip = skip * (1 << borderBottom);

	if (border != 0 &&
		(borderLeftSkip > size ||
		borderRightSkip > size ||
		borderTopSkip > size ||
		borderBottomSkip > size))
	{
		return;
	}

	// Generate a standard x by y grid of indices for a triangle strip
	// use degenerate indices to keep winding in the correct order
	std::vector<unsigned int> indices;
	int j=0;
	for (int y=0; y<size; y+=skip)
	{
		for (int x=0; x<=size; x+=skip)
		{
			int i = 0;
			// Check which way we are working across the grid
			// alter the index order accordingly
			bool up = (y % (2 * skip) == 0);
			if (up)
			{
				i = (size - x + y * (size + 1));
			}
			else
			{
				i = (x + y * (size + 1));
			}

			// Add index for each side of the strip
			indices.push_back(i);
			indices.push_back(i + (size + 1) * skip);
			
			// Add degenerate index when needed
			if (x == size)
			{
				indices.push_back(i + (size + 1) * skip);
			}
		}
	}

	// Special case for the single square
	if (indices.size() % 2 == 1)
	{
		indices.pop_back();
	}

	// Generate a mapping from the indices to the actual world indices
	// for when the patch size is larger than the index size.
	// Also move indices that don't exist in border squares.
	// Done here to keep the above code simpler.
	unsigned int *mappingIndices = new unsigned int[(size + 1) * (size + 1)];
	unsigned int *currentMappingIndex = mappingIndices;
	unsigned int currentMappingCount = 0;
	unsigned int lastGoodXTop, lastGoodXBottom, lastGoodYLeft, lastGoodYRight;
	for (int y=0; y<=size; y+=1)
	{
		// Record last possible y border index
		if (y % borderLeftSkip == 0) lastGoodYLeft = currentMappingCount;
		if (y % borderRightSkip == 0) lastGoodYRight = currentMappingCount;

		for (int x=0; x<=size; x+=1, currentMappingIndex++, currentMappingCount++)
		{
			// Record last possible x border index
			if (x % borderTopSkip == 0) lastGoodXTop = currentMappingCount;
			if (x % borderBottomSkip == 0) lastGoodXBottom = currentMappingCount;

			// Set the index for case with no border
			*currentMappingIndex = currentMappingCount;

			// Move indices if we are on a border case
			// and not only drawing one triangle
			if (((border & BorderLeft) > 0) && x == 0)
			{
				*currentMappingIndex = lastGoodYLeft;
			}
			if (((border & BorderRight) > 0) && x == size)
			{
				*currentMappingIndex = lastGoodYRight + size;
			}
			if (((border & BorderBottom) > 0) && y == 0)
			{
				*currentMappingIndex = lastGoodXBottom;
			}
			if (((border & BorderTop) > 0) && y == size)
			{
				*currentMappingIndex = lastGoodXTop;
			}
		}
		currentMappingCount += totalsize - size;
	}

	// Put the calculated indices into a unsigned array for OpenGL access
	size_ = (int) indices.size();
	if (!indices_) indices_ = new unsigned int[size_];
	for (int i=0; i<size_; i++)
	{
		int j = indices[i];
		DIALOG_ASSERT(!(j <0 || j > (size + 1) * (size + 1)));
		indices_[i] = mappingIndices[j];
	}
	delete [] mappingIndices;

	// Store this array in a vertex buffer (if available)
	if (GLStateExtension::hasVBO() &&
		!OptionsDisplay::instance()->getNoWaterBuffers())
	{
		delete bufferObject_;
		bufferObject_ = new GLVertexBufferObject(true);
		bufferObject_->init_data(size_ * sizeof(unsigned int), indices_, GL_STATIC_DRAW);
	}
}
