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

#include <water/Water2Patch.h>
#include <water/Water2Constants.h>
#include <common/Logger.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLVertexBufferObject.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLInfo.h>

Water2Patch::Water2Patch() : 
	data_(0), size_(0), bufferObject_(0)
{
}

Water2Patch::~Water2Patch()
{
	delete [] data_;
}

static Vector getPosition(Water2Points &heights, 
	int x, int y, int startX, int startY, int totalSize)
{
	int currentX = startX + x;
	int currentY = startY + y;
	int currentXNormalized = currentX % totalSize;
	int currentYNormalized = currentY % totalSize;
	if (currentXNormalized < 0) currentXNormalized += totalSize;
	if (currentYNormalized < 0) currentYNormalized += totalSize;

	int position = currentXNormalized + currentYNormalized * totalSize;
	DIALOG_ASSERT(position >= 0 && position < totalSize * totalSize);

	Vector &height = heights.getPoint(currentXNormalized, currentYNormalized);

	Vector result(
		height[0] + float(currentX) * 2.0f,
		height[1] + float(currentY) * 2.0f,
		height[2]);
	return result;
}

void Water2Patch::generate(Water2Points &heights,
	int size, int totalSize,
	int posX, int posY,
	float waterHeight)
{
	size_ = size;
	if (!data_) data_ = new Data[(size + 1) * (size + 1)];

	int startX = posX * size;
	int startY = posY * size;

	{
		Data *data = data_;
		for (int y=0; y<=size; y++)
		{
			for (int x=0; x<=size; x++, data++)
			{
				// Calculate the position
				Vector position = getPosition(heights, x, y, startX, startY, totalSize);

				// Set the position
				data->x = position[0];
				data->y = position[1];
				data->z = position[2];
			}
		}

		data = data_;
		for (int y=0; y<=size; y++)
		{
			for (int x=0; x<=size; x++, data++)
			{
				// Calculate the normal
				Vector current(data->x, data->y, data->z);
				Vector other1, other2, other3, other4;

				if (x<size) other1 = Vector((data+1)->x, (data+1)->y, (data+1)->z);
				else other1 = getPosition(heights, x + 1, y, startX, startY, totalSize);
				
				if (y<size) other2 = Vector((data+size+1)->x, (data+size+1)->y, (data+size+1)->z);
				else other2 = getPosition(heights, x, y + 1, startX, startY, totalSize);

				if (x>0) other3 = Vector((data-1)->x, (data-1)->y, (data-1)->z);
				else other3 = getPosition(heights, x - 1, y, startX, startY, totalSize);

				if (y>0) other4 = Vector((data-size-1)->x, (data-size-1)->y, (data-size-1)->z);
				else other4 = getPosition(heights, x, y - 1, startX, startY, totalSize);

				Vector dir1 = other1 - current;
				Vector dir2 = other2 - current;
				Vector dir3 = other3 - current;
				Vector dir4 = other4 - current;

				Vector normal1 = dir1 * dir2;
				Vector normal3 = dir3 * dir4;

				Vector normal = (normal1 + normal3).Normalize();

				// Set the normal
				data->nx = normal[0];
				data->ny = normal[1];
				data->nz = normal[2];
			}
		}
	}

	if (GLStateExtension::hasVBO() &&
		!OptionsDisplay::instance()->getNoWaterBuffers())
	{
		delete bufferObject_;
		bufferObject_ = new GLVertexBufferObject();
		bufferObject_->init_data((size + 1) * (size + 1) * sizeof(Data), data_, GL_STREAM_DRAW);
	}
}

void Water2Patch::draw(Water2PatchIndexs &indexes, int indexPosition, int borders)
{
	Water2PatchIndex &index = indexes.getIndex(indexPosition, borders);

	// No triangles
	GLInfo::addNoTriangles(index.getSize());

	// draw
	draw(index);
}

void Water2Patch::draw(Water2PatchIndex &index)
{
	// Map data to draw
	float *data = 0;
	if (bufferObject_)
	{
		bufferObject_->bind();
	}
	else
	{
		data = &data_[0].x;
	}

	// Vertices On
	glVertexPointer(3, GL_FLOAT, sizeof(Data), data);

	// Normals On
	glNormalPointer(GL_FLOAT, sizeof(Data), data + 3);

	// Unmap data to draw
	if (bufferObject_)
	{
		bufferObject_->unbind();
	}

	// Map indices to draw
	unsigned int *indices = 0;
	if (index.getBufferObject())
	{
		index.getBufferObject()->bind();
	}
	else
	{
		indices = index.getIndices();
	}

	// Draw elements
	glDrawElements(GL_TRIANGLE_STRIP, 
		index.getSize(), 
		GL_UNSIGNED_INT, 
		indices);

	if (index.getBufferObject())
	{
		index.getBufferObject()->unbind();
	}
}

Water2Patch::Data *Water2Patch::getData(int x, int y)
{
	DIALOG_ASSERT(x >= 0 && y >= 0 &&
		x <= size_ && y <= size_);
	return &data_[x + y * (size_ + 1)];
}
