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
	visibilityIndex_(-1)
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

	position_ = Vector(x_ + 32, y_ + 32, 5);

	int mapWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	int mapHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapHeight();

	if (x >= 0 && y >= 0 &&
		x < mapWidth && y < mapHeight)
	{
		HeightMap::HeightData *heightData = &ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getHeightMap().getHeightData()[x + (y * (mapWidth + 1))];
		heightMapData_ = heightData->position[0].getInternalData();
	}
}

void LandVisibilityPatch::setVisible(Vector &cameraPos, bool visible)
{ 
	visible_ = visible;


	if (visible)
	{
		if  (heightMapData_)
		{
			visibilityIndex_ = 6;
			if (!OptionsDisplay::instance()->getNoWaterLOD())
			{
				float distance = (cameraPos - position_).Magnitude();
				visibilityIndex_ = int(distance - 50.0f) / 70;
				visibilityIndex_ = MAX(0, MIN(visibilityIndex_, 6));
			}

			VisibilityPatchGrid::instance()->addVisibleLandPatch(this);
		}
		else
		{
			VisibilityPatchGrid::instance()->addVisibleSurroundPatch(this);
		}
	}
}

void LandVisibilityPatch::drawSurround()
{
	//glPushMatrix();

	//glTranslatef((float) x_, (float) y_, 0.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	if (GLStateExtension::hasMultiTex())
	{
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, 0.0f);
		}
	}
	glVertex2i(x_ + 0, y_ + 0);

	glTexCoord2f(1.0f, 0.0f);
	if (GLStateExtension::hasMultiTex())
	{
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 4.0f, 0.0f);
		}
	}
	glVertex2i(x_ + 64, y_ + 0);

	glTexCoord2f(1.0f, 1.0f);
	if (GLStateExtension::hasMultiTex())
	{
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 4.0f, 4.0f);
		}
	}
	glVertex2i(x_ + 64, y_ + 64);

	glTexCoord2f(0.0f, 1.0f);
	if (GLStateExtension::hasMultiTex())
	{
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, 4.0f);
		}
	}
	glVertex2i(x_ + 0, y_ + 64);

	//glPopMatrix();
}

void LandVisibilityPatch::draw(MipMapPatchIndexs &indexes, int indexPosition, int borders)
{
	if (!heightMapData_) return;

	MipMapPatchIndex &index = indexes.getIndex(indexPosition, borders);

	// No triangles
	GLInfo::addNoTriangles(index.getSize());

	// draw
	draw(index);
}

void LandVisibilityPatch::draw(MipMapPatchIndex &index)
{
	// Vertices On
	glVertexPointer(3, GL_INT, sizeof(HeightMap::HeightData), &heightMapData_[0]);

	// Normals On
	glNormalPointer(GL_INT, sizeof(HeightMap::HeightData), &heightMapData_[3]);

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
