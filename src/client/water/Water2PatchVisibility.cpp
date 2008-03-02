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

#include <water/Water2PatchVisibility.h>
#include <water/Water2Patches.h>
#include <graph/OptionsDisplay.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/Landscape.h>
#include <landscape/Sky.h>
#include <GLSL/GLSLShaderSetup.h>
#include <GLEXT/GLCameraFrustum.h>

#include <water/Water2Constants.h>

const unsigned int Water2PatchVisibility::NotVisible = 9999;

Water2PatchVisibility::VisibilityEntry::VisibilityEntry() :
	visibilityIndex(Water2PatchVisibility::NotVisible)
{
}

Water2PatchVisibility::Water2PatchVisibility() : visibility_(0), size_(0)
{
}

Water2PatchVisibility::~Water2PatchVisibility()
{
	delete [] visibility_;
}

void Water2PatchVisibility::generate(Vector &offset, unsigned int totalSize, 
	unsigned int patchesSize, unsigned int patchSize)
{
	size_ = totalSize / patchSize;
	if (!visibility_) visibility_ = new VisibilityEntry[size_ * size_];

	int width = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	int height = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getMapWidth();
	Vector middle(width / 2, height / 2);
	float allowedDistance = 2048.0f + MAX(width, height)/2.0f;

	int i=0;
	for (int y=0; y<size_; y++)
	{
		for (int x=0; x<size_; x++, i++)
		{
			VisibilityEntry &entry = visibility_[i];

			entry.position = Vector(
				float(x) * grid_size + half_grid_size + offset[0],
				float(y) * grid_size + half_grid_size + offset[1],
				5.0f + offset[2]);
			entry.offset = Vector(
				256.0f * (x / (patchesSize / patchSize)) + offset[0],
				256.0f * (y / (patchesSize / patchSize)) + offset[1],
				offset[2]);
			entry.anyoffset = (entry.offset != Vector::getNullVector());
			entry.ignore = (((middle - entry.offset).Magnitude()) > allowedDistance);
		}
	}
}

void Water2PatchVisibility::draw(Water2Patches &patches,
	Water2PatchIndexs &indexes, Vector &cameraPosition,
	Vector landscapeSize,
	GLSLShaderSetup *waterShader)
{
	
	// Figure the visibility for all patches
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "WATER_VISIBILITY");
	int i=0;
	for (int y=0; y<size_; y++)
	{
		for (int x=0; x<size_; x++, i++)
		{
			VisibilityEntry &entry = visibility_[i];
			if (entry.ignore)
			{
				entry.visibilityIndex = indexes.getNoPositions() - 1;
				entry.onScreen = false;
				continue;
			}

			Vector &position = entry.position;
			int index = indexes.getNoPositions() - 1;
			if (!OptionsDisplay::instance()->getNoWaterLOD())
			{
				float distance = (cameraPosition - position).Magnitude();
				index = int(distance - 50.0f) / 130;
				if (OptionsDisplay::instance()->getNoWaterMovement())
				{
					index += 3;
				}
				index = MAX(0, MIN(index, indexes.getNoPositions() - 1));
			}

			entry.onScreen = GLCameraFrustum::instance()->sphereInFrustum(position, grid_size);
			entry.visibilityIndex = index;
		}
	}
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "WATER_VISIBILITY");

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "WATER_DRAW");
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Draw all patches
	i = 0;
	for (int y=0; y<size_; y++)
	{
		for (int x=0; x<size_; x++, i++)
		{
			VisibilityEntry &entry = visibility_[i];
			if (!entry.onScreen) continue;

			unsigned int index = entry.visibilityIndex;
			if (index == NotVisible) continue;

			unsigned int borders = 0;
			unsigned int leftIndex = getVisibility(x - 1, y);
			unsigned int rightIndex = getVisibility(x + 1, y);
			unsigned int topIndex = getVisibility(x, y + 1);
			unsigned int bottomIndex = getVisibility(x, y - 1);

			if (leftIndex != NotVisible && leftIndex > index) 
			{
				if (leftIndex > index + 1) continue;
				borders |= Water2PatchIndex::BorderLeft;
			}
			if (rightIndex != NotVisible && rightIndex > index)
			{
				if (rightIndex > index + 1) continue;
				borders |= Water2PatchIndex::BorderRight;
			}
			if (topIndex != NotVisible && topIndex > index) 
			{
				if (topIndex > index + 1) continue;
				borders |= Water2PatchIndex::BorderTop;
			}
			if (bottomIndex != NotVisible && bottomIndex > index) 
			{
				if (bottomIndex > index + 1) continue;
				borders |= Water2PatchIndex::BorderBottom;
			}

			if (entry.anyoffset)
			{
				glPushMatrix();
				glTranslatef(entry.offset[0], entry.offset[1], entry.offset[2]);
			}

				// Setup the texture matrix for texture 0
				if (waterShader)
				{
					Vector landfoam;
					landfoam[0] = entry.offset[0];
					landfoam[1] = entry.offset[1];
					landfoam[2] = ((entry.position[0] >= 0.0f && entry.position[1] >= 0.0f &&
						entry.position[0] <= landscapeSize[0] && entry.position[1] <= landscapeSize[1])?1.0f:0.0f);
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

				Water2Patch *patch = patches.getPatch(
					x % patches.getSize(), y % patches.getSize());
				patch->draw(indexes, index, borders);

			if (entry.anyoffset)
			{
				glPopMatrix();
			}
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "WATER_DRAW");
}
