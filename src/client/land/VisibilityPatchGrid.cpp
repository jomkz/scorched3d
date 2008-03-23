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

#include <land/VisibilityPatchGrid.h>
#include <landscape/Landscape.h>
#include <water/Water2Patches.h>
#include <sky/Sky.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLVertexBufferObject.h>
#include <GLSL/GLSLShaderSetup.h>
#include <graph/MainCamera.h>
#include <graph/OptionsDisplay.h>

VisibilityPatchGrid *VisibilityPatchGrid::instance()
{
	static VisibilityPatchGrid instance;
	return &instance;
}

VisibilityPatchGrid::VisibilityPatchGrid() : 
	landPatches_(0), waterPatches_(0), visibilityPatches_(0),
	visibleLandPatches_(0), visibleLandPatchesCount_(0), 
	allLandPatchesCount_(0), allLandPatches_(0)
{
}

VisibilityPatchGrid::~VisibilityPatchGrid()
{
}

void VisibilityPatchGrid::clear()
{
	delete [] landPatches_;
	landPatches_ = 0;

	delete [] waterPatches_;
	waterPatches_ = 0;

	delete [] visibilityPatches_;
	visibilityPatches_ = 0;

	delete [] visibleLandPatches_;
	visibleLandPatchesCount_ = 0;
	visibleLandPatches_ = 0;

	for (int i=0; i<4; i++)
	{
		delete waterVisibility_[i].visibleWaterPatches;
		waterVisibility_[i].visibleWaterPatches = 0;
		waterVisibility_[i].visibleWaterPatchesCount = 0;
	}

	delete [] allLandPatches_;
	allLandPatches_ = 0;
	allLandPatchesCount_ = 0;
}

void VisibilityPatchGrid::generate()
{
	clear();

	// Figure out how large the visible area will be
	int mapWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	int mapHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	int actualWidth = mapWidth + 4096; // The map visible area is at least 4096*4096
	int actualHeight = mapHeight + 4096;
	actualWidth = (actualWidth / 512) * 512; // The actual width and height are multiples of 512
	actualHeight = (actualHeight / 512) * 512;

	// Generate land indices
	landIndexs_.generate(32, mapWidth);

	// Find the mid point
	midX_ = -actualWidth / 2 + mapWidth / 2;
	midY_ = -actualHeight / 2 + mapHeight / 2;
	midX_ = (midX_ / 32) * 32; // Move to the nearest 32 boundry
	midY_ = (midY_ / 32) * 32;

	{
		// Divide this visible area into a set of patches
		landWidth_ = actualWidth / 32;
		landHeight_ = actualHeight / 32;

		// Create the patches
		landPatches_ = new LandVisibilityPatch[landWidth_ * landHeight_];
		visibleLandPatches_ = lastVisibleLandPatches_ =
			new LandVisibilityPatch*[landWidth_ * landHeight_];
		allLandPatches_ =
			new LandVisibilityPatch*[landWidth_ * landHeight_];

		// For each patch set it's location
		LandVisibilityPatch *currentPatch = landPatches_;
		for (int y=0; y<landHeight_; y++)
		{
			for (int x=0; x<landWidth_; x++, currentPatch++)
			{
				LandVisibilityPatch *leftPatch = (x==0?0:currentPatch-1);
				LandVisibilityPatch *rightPatch = (x==landWidth_-1?0:currentPatch+1);
				LandVisibilityPatch *topPatch = (y==0?0:currentPatch-landWidth_);
				LandVisibilityPatch *bottomPatch = (y==landHeight_-1?0:currentPatch+landWidth_);

				currentPatch->setLocation(x * 32 + midX_, y * 32 + midY_,
					leftPatch, rightPatch, topPatch, bottomPatch);
				if (currentPatch->hasData())
				{
					allLandPatches_[allLandPatchesCount_++] = currentPatch;
				}
			}
		}
	}

	{
		// Divide this visible area into a set of patches
		waterWidth_ = actualWidth / 128;
		waterHeight_ = actualHeight / 128;

		// Create the patches
		waterPatches_ = new WaterVisibilityPatch[waterWidth_ * waterHeight_];
		for (int i=0; i<4; i++)
		{
			waterVisibility_[i].visibleWaterPatches =
				new WaterVisibilityPatch*[waterWidth_ * waterHeight_];
		}

		// For each patch set it's location
		WaterVisibilityPatch *currentPatch = waterPatches_;
		for (int y=0; y<waterHeight_; y++)
		{
			for (int x=0; x<waterWidth_; x++, currentPatch++)
			{
				WaterVisibilityPatch *leftPatch = (x==0?0:currentPatch-1);
				WaterVisibilityPatch *rightPatch = (x==waterWidth_-1?0:currentPatch+1);
				WaterVisibilityPatch *topPatch = (y==0?0:currentPatch-waterWidth_);
				WaterVisibilityPatch *bottomPatch = (y==waterHeight_-1?0:currentPatch+waterWidth_);

				currentPatch->setLocation(x * 128 + midX_, y * 128 + midY_,
					leftPatch, rightPatch, topPatch, bottomPatch);
			}
		}
	}

	{
		// Devide this visible area into a set of visibility test patches
		visibilityWidth_ = actualWidth / 512;
		visibilityHeight_ = actualHeight / 512;

		// Create the set of visibility patches
		visibilityPatches_ = new VisibilityPatchQuad[visibilityWidth_ * visibilityHeight_];

		// Set the visibility location
		VisibilityPatchQuad *currentPatch = visibilityPatches_;
		for (int y=0; y<visibilityHeight_; y++)
		{
			for (int x=0; x<visibilityWidth_; x++, currentPatch++)
			{
				currentPatch->setLocation(this, x * 512 + midX_, y * 512 + midY_, 512,
					mapWidth, mapHeight);
			}
		}
	}

	surround_.generate();
}

LandVisibilityPatch *VisibilityPatchGrid::getLandVisibilityPatch(int x, int y)
{
	int realX = (x - midX_) / 32;
	int realY = (y - midY_) / 32;

	if (realX < 0 || realY < 0 ||
		realX >= landWidth_ || realY >= landHeight_) 
	{
		return 0;
	}

	return &landPatches_[realX + realY * landWidth_];
}

WaterVisibilityPatch *VisibilityPatchGrid::getWaterVisibilityPatch(int x, int y)
{
	int realX = (x - midX_) / 128;
	int realY = (y - midY_) / 128;

	if (realX < 0 || realY < 0 ||
		realX >= waterWidth_ || realY >= waterHeight_) 
	{
		return 0;
	}

	return &waterPatches_[realX + realY * waterWidth_];
}

void VisibilityPatchGrid::drawVisibility()
{
	Vector &cameraPos = 
		MainCamera::instance()->getTarget().getCamera().getCurrentPos();

	visibleLandPatchesCount_ = 0;
	lastVisibleLandPatches_ = visibleLandPatches_;
	for (int i=0; i<4; i++)
	{
		waterVisibility_[i].visibleWaterPatchesCount = 0;
		waterVisibility_[i].lastVisibleWaterPatches = waterVisibility_[i].visibleWaterPatches;
	}

	// Calculate visibility
	VisibilityPatchQuad *currentPatch = visibilityPatches_;
	for (int y=0; y<visibilityHeight_; y++)
	{
		for (int x=0; x<visibilityWidth_; x++, currentPatch++)
		{
			currentPatch->calculateVisibility(cameraPos);
		}
	}
}

void VisibilityPatchGrid::drawLand(int addIndex)
{
	if (!OptionsDisplay::instance()->getNoGLDrawElements())
	{
		// Map indices to draw
		if (landIndexs_.getBufferObject())
		{
			landIndexs_.getBufferObject()->bind();
		}

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
	}

	{
		LandVisibilityPatch **currentPatchPtr = visibleLandPatches_;
		for (int i=0; i<visibleLandPatchesCount_; i++, currentPatchPtr++)
		{
			LandVisibilityPatch *currentPatch = *currentPatchPtr;
			unsigned int index = currentPatch->getVisibilityIndex();
			if (index == -1) continue;

			unsigned int borders = 0;
			int leftIndex = currentPatch->getLeftPatch()?
				currentPatch->getLeftPatch()->getVisibilityIndex():-1;
			int rightIndex = currentPatch->getRightPatch()?
				currentPatch->getRightPatch()->getVisibilityIndex():-1;
			int topIndex = currentPatch->getTopPatch()?
				currentPatch->getTopPatch()->getVisibilityIndex():-1;
			int bottomIndex = currentPatch->getBottomPatch()?
				currentPatch->getBottomPatch()->getVisibilityIndex():-1;

			MipMapPatchIndex &landIndex = 
				landIndexs_.getIndex(index, leftIndex, rightIndex, topIndex, bottomIndex, addIndex);

			currentPatch->draw(landIndex);
		}
	}

	if (!OptionsDisplay::instance()->getNoGLDrawElements())
	{
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

		// Unmap indices to draw
		if (landIndexs_.getBufferObject())
		{
			landIndexs_.getBufferObject()->unbind();
		}
	}
}

void VisibilityPatchGrid::drawSimpleLand()
{
	if (!OptionsDisplay::instance()->getNoGLDrawElements())
	{
		// Map indices to draw
		if (landIndexs_.getBufferObject())
		{
			landIndexs_.getBufferObject()->bind();
		}

		glEnableClientState(GL_VERTEX_ARRAY);
	}

	{
		LandVisibilityPatch **currentPatchPtr = allLandPatches_;
		for (int i=0; i<allLandPatchesCount_; i++, currentPatchPtr++)
		{
			LandVisibilityPatch *currentPatch = *currentPatchPtr;
			MipMapPatchIndex &landIndex = landIndexs_.getIndex(4, 0);
			currentPatch->draw(landIndex);
		}
	}

	if (!OptionsDisplay::instance()->getNoGLDrawElements())
	{
		glDisableClientState(GL_VERTEX_ARRAY);

		// Unmap indices to draw
		if (landIndexs_.getBufferObject())
		{
			landIndexs_.getBufferObject()->unbind();
		}
	}
}

void VisibilityPatchGrid::drawSurround()
{
	surround_.draw(ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap(), true, true);
}

void VisibilityPatchGrid::drawWater(Water2Patches &patches, 
		MipMapPatchIndexs &indexes, Vector &cameraPosition, 
		Vector landscapeSize,
		GLSLShaderSetup *waterShader)
{
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "WATER_DRAW");

	if (!OptionsDisplay::instance()->getNoGLDrawElements())
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		// Map the vertex VBO
		if (patches.getBufferObject())
		{
			patches.getBufferObject()->bind();
		}

		// Map indices to draw
		if (indexes.getBufferObject())
		{
			indexes.getBufferObject()->bind();
		}
	}

	for (int w=0; w<4; w++)
	{
		Water2Patch *patch = patches.getPatch(w);

		if (!OptionsDisplay::instance()->getNoGLDrawElements())
		{
			// Map data to draw
			float *data = 0;
			if (patch->getBufferOffSet() != -1)
			{
				data = (float*) NULL + (patch->getBufferOffSet() / sizeof(unsigned int));
			}
			else
			{
				data = &patch->getInternalData()->x;
			}

			// Vertices On
			glVertexPointer(3, GL_FLOAT, sizeof(Water2Patch::Data), data);

			// Normals On
			glNormalPointer(GL_FLOAT, sizeof(Water2Patch::Data), data + 3);
		}

		// Draw all patches
		WaterVisibilityPatch **currentPatchPtr = waterVisibility_[w].visibleWaterPatches;
		for (int i=0; i<waterVisibility_[w].visibleWaterPatchesCount; i++, currentPatchPtr++)
		{
			WaterVisibilityPatch *currentPatch = *currentPatchPtr;

			unsigned int index = currentPatch->getVisibilityIndex();
			if (index == -1) continue;

			unsigned int borders = 0;
			int leftIndex = currentPatch->getLeftPatch()?
				currentPatch->getLeftPatch()->getVisibilityIndex():-1;
			int rightIndex = currentPatch->getRightPatch()?
				currentPatch->getRightPatch()->getVisibilityIndex():-1;
			int topIndex = currentPatch->getTopPatch()?
				currentPatch->getTopPatch()->getVisibilityIndex():-1;
			int bottomIndex = currentPatch->getBottomPatch()?
				currentPatch->getBottomPatch()->getVisibilityIndex():-1;

			MipMapPatchIndex &patchIndex = 
				indexes.getIndex(index, leftIndex, rightIndex, topIndex, bottomIndex, 0);

			glPushMatrix();
			glTranslatef(
				currentPatch->getOffset()[0],
				currentPatch->getOffset()[1], 
				currentPatch->getOffset()[2]);

			// Setup the texture matrix for texture 0
			if (waterShader)
			{
				Vector landfoam;
				landfoam[0] = currentPatch->getOffset()[0];
				landfoam[1] = currentPatch->getOffset()[1];
				landfoam[2] = ((currentPatch->getPosition()[0] >= 0.0f && 
					currentPatch->getPosition()[1] >= 0.0f &&
					currentPatch->getPosition()[0] <= landscapeSize[0] && 
					currentPatch->getPosition()[1] <= landscapeSize[1])?1.0f:0.0f);
				waterShader->set_uniform("landfoam", landfoam);

				// Set lighting position
				// done after the translation
				Landscape::instance()->getSky().getSun().setLightPosition(true);

				// get projection and modelview matrix
				// done after the translation
				static float proj[16], model[16];
				glGetFloatv(GL_PROJECTION_MATRIX, proj);
				glGetFloatv(GL_MODELVIEW_MATRIX, model);

				// Setup the texture matrix for texture 1
				glActiveTexture(GL_TEXTURE1);
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				glTranslatef(0.5f,0.5f,0.0f);
				glScalef(0.5f,0.5f,1.0f);
				glMultMatrixf(proj);
				glMultMatrixf(model);
				glMatrixMode(GL_MODELVIEW);

				// Reset to texture 0
				glActiveTexture(GL_TEXTURE0);
			}

			patch->draw(patchIndex);

			glPopMatrix();
		}
	}

	if (!OptionsDisplay::instance()->getNoGLDrawElements())
	{
		// Unmap data to draw
		if (patches.getBufferObject())
		{
			patches.getBufferObject()->unbind();
		}

		// Unmap indices to draw
		if (indexes.getBufferObject())
		{
			indexes.getBufferObject()->unbind();
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "WATER_DRAW");
}
