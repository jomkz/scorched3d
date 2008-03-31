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
#include <graph/MainCamera.h>
#include <client/ScorchedClient.h>
#include <common/Logger.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLInfo.h>
#include <GLW/GLWFont.h>

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

static float getHeight(int x, int y)
{
	int mapWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap().getMapWidth();
	return ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap().getHeightData()[x + (y * (mapWidth + 1))].floatPosition[2];
}

static float calculateError(int x1, int x2, int y1, int y2,
	float x1y1, float x2y2, float x1y2, float x2y1)
{
	if (x2 - x1 <= 1) return 0.0f;

	int midx = (x1 + x2) / 2;
	int midy = (y1 + y2) / 2;
	float actualheight = getHeight(midx, midy);

	float approxheight1 = (x1y1 + x2y2) / 2.0f;
	float approxheight2 = (x1y2 + x2y1) / 2.0f;
	float approxheight3 = (x1y1 + x1y2) / 2.0f;
	float approxheight4 = (x1y1 + x2y1) / 2.0f;
	float approxheight5 = (x1y2 + x2y2) / 2.0f;
	float approxheight6 = (x2y1 + x2y2) / 2.0f;

	float heightdiff1 = fabs(approxheight1 - actualheight);
	float heightdiff2 = fabs(approxheight2 - actualheight);
	
	float errorChild1 = calculateError(x1, midx, y1, midy,
		x1y1, approxheight1, approxheight3, approxheight4);
	float errorChild2 = calculateError(midx, x2, y1, midy,
		approxheight4, approxheight6, approxheight1, x2y1);
	float errorChild3 = calculateError(x1, midx, midy, y2,
		approxheight3, approxheight5, x1y2, approxheight2);
	float errorChild4 = calculateError(midx, x2, midy, y2,
		approxheight2, x2y2, approxheight5, approxheight6);

	float errorChildren = MAX(errorChild1, MAX(errorChild2, MAX(errorChild3, errorChild4)));
	float totalError = MAX(errorChildren, MAX(heightdiff1, heightdiff2));
	return totalError;
}

void LandVisibilityPatch::setLocation(int x, int y,
	LandVisibilityPatch *leftPatch, 
	LandVisibilityPatch *rightPatch, 
	LandVisibilityPatch *topPatch, 
	LandVisibilityPatch *bottomPatch)
{
	int mapWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	int mapHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapHeight();

	DIALOG_ASSERT(x >= 0 && y >= 0 &&
		x < mapWidth && y < mapHeight);

	// Set location and neighbors
	x_ = x; y_ = y;
	leftPatch_ = leftPatch;
	rightPatch_ = rightPatch;
	topPatch_ = topPatch;
	bottomPatch_ = bottomPatch;

	// Set pointers to heightmap
	dataSize_ = (mapWidth + 1) * (mapHeight + 1);
	HeightMap::HeightData *heightData = &ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap().getHeightData()[x + (y * (mapWidth + 1))];
	heightMapData_ = (float *) &heightData->floatPosition;

	maxHeight_ = 0.0f;
	minHeight_ = 100000.0f;
	for (int i=0; i<=5; i++)
	{
		float error = 0.0f;
		if (i>0)
		{
			int skip = 1 << i;
			for (int y1=y_; y1<y_+32; y1+=skip)
			{
				for (int x1=x_; x1<x_+32; x1+=skip)
				{
					int x2 = x1 + skip;
					int y2 = y1 + skip;

					float x1y1 = getHeight(x1, y1);
					float x2y2 = getHeight(x2, y2);
					float x1y2 = getHeight(x1, y2);
					float x2y1 = getHeight(x2, y1);

					float thisError = calculateError(x1, x2, y1, y2,
						x1y1, x2y2, x1y2, x2y1);
					error = MAX(error, thisError);		

					if (x1y1 > maxHeight_) maxHeight_ = x1y1;
					if (x1y1 < minHeight_) minHeight_ = x1y1;
				}
			}
		}

		indexErrors_[i] = error;
	}

	float heightRange = maxHeight_ - minHeight_;
	boundingSize_ = MAX(32.0f, heightRange) * 1.25f;
	position_ = Vector(float(x_ + 16), float(y_ + 16), 
		heightRange / 2.0f + minHeight_);
}

bool LandVisibilityPatch::setVisible(Vector &cameraPos)
{ 
	visible_ = true;
	DIALOG_ASSERT(heightMapData_);

	float distance = (cameraPos - position_).Magnitude();

	float maxError = 8.0f;
	if (distance < 32.0f) maxError = 1.0f;
	else if (distance < 64.0f) maxError = 3.0f;
	else if (distance < 256.0f) maxError = 5.0f;

	visibilityIndex_ = 0;
	if (!OptionsDisplay::instance()->getNoLandLOD())
	{
		for (int i=0; i<=5; i++)
		{
			if (indexErrors_[i] > maxError) break;
			visibilityIndex_ = i;
		}
	}

	return true;
}

void LandVisibilityPatch::setNotVisible()
{
	visible_ = false;
	DIALOG_ASSERT(heightMapData_);
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
			index.getMinIndex(), 
			index.getMaxIndex(),
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
					glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, data + 6);
					if (GLStateExtension::getTextureUnits() > 2)
					{
						glMultiTexCoord2fvARB(GL_TEXTURE2_ARB, data + 8);
					}
				}
			}
		glEnd();
	}
}

void LandVisibilityPatch::drawLODLevel(MipMapPatchIndex &index)
{
	if (OptionsDisplay::instance()->getDrawLines()) glPolygonMode(GL_FRONT, GL_FILL);

	Vector red(1.0f, 0.0f, 0.0f);
	Vector yellow(1.0f, 1.0f, 0.0f);
	GLWFont::instance()->getGameFont()->drawBilboard(red, 1.0f, 3.0f, 
		position_[0], position_[1], position_[2], 
		S3D::formatStringBuffer("%i", visibilityIndex_));

	if ((MainCamera::instance()->getCamera().getLookAt() - position_).Magnitude() < 20.0f)
	{
		for (int i=0; i<index.getSize(); i++)
		{
			float *data = heightMapData_ + 
				(sizeof(HeightMap::HeightData) / 4 * index.getIndices()[i]);

			GLWFont::instance()->getGameFont()->drawBilboard(yellow, 1.0f, 1.0f, 
				data[0], data[1], data[2] + i * 1.5f, 
				S3D::formatStringBuffer("%i", i));
		}
	}

	if (OptionsDisplay::instance()->getDrawLines()) glPolygonMode(GL_FRONT, GL_LINE);
}
