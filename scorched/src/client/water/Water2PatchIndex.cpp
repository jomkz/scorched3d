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

#include <water/Water2PatchIndex.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLVertexBufferObject.h>
#include <GLEXT/GLStateExtension.h>
#include <vector>

Water2PatchIndex::Water2PatchIndex() : 
	indices_(0), size_(0), bufferObject_(0)
{
}

Water2PatchIndex::~Water2PatchIndex()
{
	delete [] indices_;
}

void Water2PatchIndex::generate(int size, int skip, unsigned int border)
{
	std::vector<unsigned int> indices;

	int j=0;
	for (int y=0; y<size; y+=skip)
	{
		for (int x=0; x<=size; x+=skip)
		{
			int i = 0;
			bool up = (y % (2 * skip) == 0);
			bool left = (x % (2 * skip) == 0);
			if (up)
			{
				i = (size - x + y * (size + 1));
			}
			else
			{
				i = (x + y * (size + 1));
			}

			int iLeft = i;
			int iRight = i;
			if (border & BorderLeft)
			{
				if (x == size && up)
				{
					iRight += (size + 1) * skip;
				}
				if (x == 0 && !up)
				{
					iLeft += (size + 1) * skip;
				}
			}
			if (border & BorderRight)
			{
				if (x == 0 && up)
				{
					iRight -= (size + 1) * skip;
				}
				if (x == size && !up)
				{
					iLeft -= (size + 1) * skip;
				}
			}
			if (border & BorderBottom)
			{
				if (y == 0 && !left)
				{
					iLeft += skip;
				}
			}
			if (border & BorderTop)
			{
				if (y == (size - skip) && !left)
				{
					iRight += skip;
				}
			}

			indices.push_back(iLeft);
			indices.push_back(iRight + (size + 1) * skip);
			
			if (x == size)
			{
				indices.push_back(iRight + (size + 1) * skip);
			}
		}
	}

	// Special case for the single square
	if (indices.size() % 2 == 1)
	{
		indices.pop_back();
	}

	size_ = (int) indices.size();
	if (!indices_) indices_ = new unsigned int[size_];
	for (int i=0; i<size_; i++) indices_[i] = indices[i];

	if (GLStateExtension::hasVBO() &&
		!OptionsDisplay::instance()->getNoWaterBuffers())
	{
		delete bufferObject_;
		bufferObject_ = new GLVertexBufferObject(true);
		bufferObject_->init_data(size_ * sizeof(unsigned int), indices_, GL_STATIC_DRAW);
	}
}
