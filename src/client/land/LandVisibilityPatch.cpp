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

#include <land/LandVisibilityPatch.h>
#include <land/VisibilityPatchGrid.h>
#include <landscapemap/LandscapeMaps.h>
#include <geomipmap/MipMapPatchIndexs.h>
#include <graph/OptionsDisplay.h>
#include <client/ScorchedClient.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLVertexBufferObject.h>
#include <GLEXT/GLInfo.h>

LandVisibilityPatch::LandVisibilityPatch() : 
	visible_(false), heightMapData_(0),
	leftPatch_(0), rightPatch_(0),
	topPatch_(0), bottomPatch_(0),
	visibilityIndex_(-1),
	dataSize_(0)
{
}

LandVisibilityPatch::~LandVisibilityPatch()
{
}

void LandVisibilityPatch::setLocation(int x, int y,
	LandVisibilityPatch *leftPatch, 
	LandVisibilityPatch *rightPatch, 
	LandVisibilityPatch *topPatch, 
	LandVisibilityPatch *bottomPatch)
{
	x_ = x; y_ = y;
	leftPatch_ = leftPatch;
	rightPatch_ = rightPatch;
	topPatch_ = topPatch;
	bottomPatch_ = bottomPatch;

	position_ = Vector(x_ + 16, y_ + 16, 5);

	int mapWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	int mapHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapHeight();

	dataSize_ = (mapWidth + 1) * (mapHeight + 1);

	if (x >= 0 && y >= 0 &&
		x < mapWidth && y < mapHeight)
	{
		HeightMap::HeightData *heightData = &ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getHeightMap().getHeightData()[x + (y * (mapWidth + 1))];
		heightMapData_ = (float *) &heightData->floatPosition;
	}
}

void LandVisibilityPatch::setVisible(Vector &cameraPos, bool visible)
{ 
	visible_ = visible;

	DIALOG_ASSERT(heightMapData_);

	if (visible)
	{
		visibilityIndex_ = 3;
		if (!OptionsDisplay::instance()->getNoWaterLOD())
		{
			float distance = (cameraPos - position_).Magnitude();

			if (distance < 512) 
			{
				visibilityIndex_ = (int(distance) - 50) / 40;
				visibilityIndex_ = MAX(0, MIN(visibilityIndex_, 3));
			}
			else
			{
				visibilityIndex_ = (int(distance) - (512 - 140)) / 40;
				visibilityIndex_ = MAX(0, MIN(visibilityIndex_, 5));
			}
		}

		VisibilityPatchGrid::instance()->addVisibleLandPatch(this);
	}
}

void LandVisibilityPatch::draw(MipMapPatchIndex &index)
{
	if (!heightMapData_) return;

	// Number triangles
	GLInfo::addNoTriangles(index.getSize() - 2);

	if (!OptionsDisplay::instance()->getNoGLDrawElements())
	{
		// Vertices On
		glVertexPointer(3, GL_FLOAT, sizeof(HeightMap::HeightData), &heightMapData_[0]);

		// Normals On
		glNormalPointer(GL_FLOAT, sizeof(HeightMap::HeightData), &heightMapData_[3]);

		// Tex Coords
		if (GLStateExtension::hasMultiTex())
		{
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glTexCoordPointer(2, GL_FLOAT, sizeof(HeightMap::HeightData), &heightMapData_[6]);
			if (GLStateExtension::getTextureUnits() > 2)
			{
				glClientActiveTextureARB(GL_TEXTURE2_ARB);
				glTexCoordPointer(2, GL_FLOAT, sizeof(HeightMap::HeightData), &heightMapData_[8]);
			}
		}
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexCoordPointer(2, GL_FLOAT, sizeof(HeightMap::HeightData), &heightMapData_[6]);

		// Map indices to draw
		unsigned short *indices = 0;
		if (index.getBufferOffSet() != -1)
		{
			indices = (unsigned short *) NULL + (index.getBufferOffSet() / sizeof(unsigned short));
		}
		else
		{
			indices = index.getIndices();
		}

		// Draw elements
		glDrawRangeElements(GL_TRIANGLE_STRIP, 
			0, 
			dataSize_,
			index.getSize(), 
			GL_UNSIGNED_SHORT, 
			indices);
	}
	else
	{
		glBegin(GL_TRIANGLE_STRIP);
			for (int i=0; i<index.getSize(); i++)
			{
				float *data = heightMapData_ + 
					(sizeof(HeightMap::HeightData) / 4 * index.getIndices()[i]);

				glVertex3fv(data);
				glNormal3fv(data + 3);
				glTexCoord2fv(data + 6);

				if (GLStateExtension::hasMultiTex())
				{
					glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, data + 8);
					if (GLStateExtension::getTextureUnits() > 2)
					{
						glMultiTexCoord2fvARB(GL_TEXTURE2_ARB, data + 6);
					}
				}
			}
		glEnd();
	}
}
