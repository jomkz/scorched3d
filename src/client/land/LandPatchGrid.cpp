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

#include <land/LandPatchGrid.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <GLEXT/GLStateExtension.h>

LandPatchGrid::LandPatchGrid() : patches_(0), visibilityPatches_(0)
{
}

LandPatchGrid::~LandPatchGrid()
{
}

void LandPatchGrid::clear()
{
	delete [] patches_;
	patches_ = 0;

	delete [] visibilityPatches_;
	visibilityPatches_ = 0;
}

void LandPatchGrid::generate()
{
	clear();

	// Figure out how large the visible area will be
	int mapWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	int mapHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	int actualWidth = mapWidth + 1024; // The map visible area is at least 1024*1024
	int actualHeight = mapHeight + 1024;
	actualWidth = (actualWidth / 512) * 512; // The actual width and height are multiples of 512
	actualHeight = (actualHeight / 512) * 512;

	// Generate land indices
	landIndexs_.generate(64, mapWidth);

	// Find the mid point
	midX_ = -actualWidth / 2 + mapWidth / 2;
	midY_ = -actualHeight / 2 + mapHeight / 2;
	midX_ = (midX_ / 64) * 64; // Move to the nearest 64 boundry
	midY_ = (midY_ / 64) * 64;

	{
		// Divide this visible area into a set of patches
		width_ = actualWidth / 64;
		height_ = actualHeight / 64;

		// Create the patches
		patches_ = new LandPatch[width_ * height_];

		// For each patch set it's location
		LandPatch *currentPatch = patches_;
		for (int y=0; y<height_; y++)
		{
			for (int x=0; x<width_; x++, currentPatch++)
			{
				currentPatch->setLocation(x * 64 + midX_, y * 64 + midY_);
			}
		}
	}

	{
		// Devide this visible area into a set of visibility test patches
		visibilityWidth_ = actualWidth / 512;
		visibilityHeight_ = actualHeight / 512;

		// Create the set of visibility patches
		visibilityPatches_ = new LandVisibilityPatch[visibilityWidth_ * visibilityHeight_];

		// Set the visibility location
		LandVisibilityPatch *currentPatch = visibilityPatches_;
		for (int y=0; y<visibilityHeight_; y++)
		{
			for (int x=0; x<visibilityWidth_; x++, currentPatch++)
			{
				currentPatch->setLocation(this, x * 512 + midX_, y * 512 + midY_, 512);
			}
		}
	}
}


LandPatch *LandPatchGrid::getLandPatch(int x, int y)
{
	int realX = (x - midX_) / 64;
	int realY = (y - midY_) / 64;

	if (realX < 0 || realY < 0 ||
		realX >= width_ || realY >= height_) 
	{
		return 0;
	}

	return &patches_[realX + realY * width_];
}

void LandPatchGrid::draw()
{
	drawVisibility();
	drawLand();
}

void LandPatchGrid::drawVisibility()
{
	// Calculate visibility
	LandVisibilityPatch *currentPatch = visibilityPatches_;
	for (int y=0; y<visibilityHeight_; y++)
	{
		for (int x=0; x<visibilityWidth_; x++, currentPatch++)
		{
			currentPatch->calculateVisibility();
		}
	}
}

void LandPatchGrid::drawLand()
{
	glPushMatrix();

	// Scale from fixed to floats (1 fixed unit is 10000)
	glScalef(0.0001f, 0.0001f, 0.0001f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (GLStateExtension::hasMultiTex())
	{
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			glClientActiveTextureARB(GL_TEXTURE2_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	{
		int visibleCount = 0, notVisibleCount = 0;
		LandPatch *currentPatch = patches_;
		for (int y=0; y<height_; y++)
		{
			for (int x=0; x<width_; x++, currentPatch++)
			{
				if (currentPatch->getVisible())
				{
					visibleCount ++;
					currentPatch->draw(landIndexs_, 0, 0);
				}
				else
				{
					notVisibleCount ++;
				}
			}
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if (GLStateExtension::hasMultiTex())
	{
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		if (GLStateExtension::getTextureUnits() > 2)
		{
			glClientActiveTextureARB(GL_TEXTURE2_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();
}
