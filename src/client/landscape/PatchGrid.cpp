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
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLDynamicVertexArray.h>
#include <GLEXT/GLCamera.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <tank/TankContainer.h>
#include <target/TargetLife.h>
#include <landscape/PatchGrid.h>
#include <landscape/TriNodePool.h>

PatchGrid::PatchGrid(HeightMap *hMap, int patchSize) :
	hMap_(hMap), lastPos_(-1, -2, -3), patchSize_(patchSize),
	simulationTime_(0.0f), drawnPatches_(0), patches_(0),
	width_(0), height_(0)
{
}

PatchGrid::~PatchGrid()
{
	delete [] patches_;
}

void PatchGrid::generate()
{
	if (hMap_->getMapWidth() % patchSize_ != 0 ||
		hMap_->getMapHeight() % patchSize_ != 0)
	{
		S3D::dialogMessage("Scorched3D", S3D::formatStringBuffer(
			"Landscape dimensions must be a factor of %i",
			patchSize_));
	}

	delete [] patches_;
	
	width_ = (hMap_->getMapWidth()+1) / patchSize_;
	height_ = (hMap_->getMapHeight()+1) / patchSize_;
	coord_.generate(hMap_);
	
	patches_ = new Patch*[width_ * height_];
	for (int y=0; y<height_; y++)
	{
		for (int x=0; x<width_; x++)
		{
			patches_[x + y * width_] = 
				new Patch(hMap_, &coord_,
					x * patchSize_, 
					y * patchSize_, 
					patchSize_);
		}
	}
}

void PatchGrid::simulate(float frameTime)
{
	DIALOG_ASSERT(patches_);
	const float SimulationTimeStep = 0.25f;

	// Only-recalculate 1/SimulationTimeStep variances per second
	simulationTime_ += frameTime;
	while (simulationTime_ > SimulationTimeStep)
	{
		simulationTime_ -= SimulationTimeStep;

		// Recalculate the first un-calculated variance
		Patch **patch = patches_;
		int totalChanged = 0;
		int patchTotal = width_ * height_;
		for (int p=0; p<patchTotal; p++)
		{			
			if ((*patch)->getRecalculate())
			{
				(*patch)->getRecalculate() = false;
				(*patch)->computeVariance();
				if (++totalChanged >= 3) break;
			}
			patch++;
		}
	}
}

void PatchGrid::reset(ProgressCounter *counter)
{
	DIALOG_ASSERT(patches_);
	if (counter) counter->setNewOp("Patch Grid");

	// Totaly recalculate all patch grid variances
	Patch **patch = patches_;
	int totalPatches = width_ * height_;
	for (int p=0; p<totalPatches; p++)
	{			
		(*patch)->getRecalculate() = false;
		(*patch)->computeVariance();

		if (counter) counter->setNewPercentage(
			100.0f * float(p) / float(totalPatches));

		patch++;
	}

	// Recalculate all the variances due to tanks positions
	recalculateTankVariance();

	// Make sure the next frame has everything reset
	// and recalculated
	lastPos_ = Vector(-1, -2, -3);
}

void PatchGrid::recalculateTankVariance()
{
	if (!patches_) return;

	// Mark all patches as normal (non-tank) variance levels
	Patch **patch = patches_;
	for (int p=0; p<width_ * height_; p++)
	{
		(*patch)->getForceVariance() = false;
		patch++;
	}

	// Mark any patches with tanks on as better variance levels
	// Ensure that any patches with tanks on them are drawn with
	// a very low varaince
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Target *current = (*itor).second;
		if (current->getAlive())
		{
			float posX = current->getLife().getFloatPosition()[0];
			float posY = current->getLife().getFloatPosition()[1];

			posX /= float(patchSize_);
			posY /= float(patchSize_);

			int posXI = (int) posX;
			int posYI = (int) posY;

			if (posXI < 0) posXI = 0;
			else if (posXI >= width_) posXI = width_ - 1;
			if (posYI < 0) posYI = 0;
			else if (posYI >= height_) posYI = height_ - 1;

			patches_[posXI + posYI * width_]->getForceVariance() = true;
		}
	}
}

void PatchGrid::recalculate(int posX, int posY, int dist)
{
	DIALOG_ASSERT(patches_);

	// Make sure the next frame has everything reset
	// and recalculated
	lastPos_ = Vector(-1, -2, -3);

	// Tag the any patches that may have changed
	// so their variances are checked
	Patch **patch = patches_;
	for (int p=0; p<width_ * height_; p++)
	{
		Patch *current = (*patch);

		int distX = abs(current->getX() - posX);
		int distY = abs(current->getY() - posY);
		if (distX < dist + current->getWidth() &&
			distY < dist + current->getWidth())
		{
			current->getRecalculate() = true;
		}

		patch++;
	}

	// Recalculate all the variances due to tanks positions
	recalculateTankVariance();
}

void PatchGrid::tesselate()
{
	if (OptionsDisplay::instance()->getNoTessalation()) return;

	// Only need to recalculate patches if the viewing camera has changed
	// position
	Vector &pos = GLCamera::getCurrentCamera()->getCurrentPos();
	if (fabs(pos[0] - lastPos_[0]) < 1.0f &&
		fabs(pos[1] - lastPos_[1]) < 1.0f &&
		fabs(pos[2] - lastPos_[2]) < 1.0f)
	{
		return;
	}
	lastPos_ = pos;

	// Link all the patches together.
	TriNodePool::instance()->reset();
	Patch **patch = patches_;
	for (int y=0; y<height_; y++)
	{
		for (int x=0; x<width_; x++)
		{
			(*patch)->reset();

			if ( x < width_-1 ) (*patch)->getLeftTri()->LeftNeighbor = (*(patch+1))->getRightTri();
			else (*patch)->getLeftTri()->LeftNeighbor = 0;

			if ( x > 0 ) (*patch)->getRightTri()->LeftNeighbor = (*(patch-1))->getLeftTri();
			else (*patch)->getRightTri()->LeftNeighbor = 0;	

			if ( y > 0 ) (*patch)->getLeftTri()->RightNeighbor = (*(patch-width_))->getRightTri();
			else (*patch)->getLeftTri()->RightNeighbor = 0;

			if ( y < height_-1 ) (*patch)->getRightTri()->RightNeighbor = (*(patch+width_))->getLeftTri();
			else (*patch)->getRightTri()->RightNeighbor = 0;

			patch++;
		}
	}

	// Tessalate
	Vector point;
	patch = patches_;
	for (int p=0; p<width_ * height_; p++)
	{
		if ((*patch)->getVisible())
		{
			point = (*patch)->getMidPoint();
			point[2] = float(hMap_->getHeight((int) point[0], (int) point[1]).asFloat());
			point -= pos;

			// Ramp is stored in a factor of 10 i.e. devide by 10
			float distance = point.Magnitude();
			float start = 
				float(OptionsDisplay::instance()->getRoamVarianceStart());
			float ramp = distance * 
				float(OptionsDisplay::instance()->getRoamVarianceRamp()) / 10.0f;
			float tank = 0.0f;
			if ((*patch)->getForceVariance())
			{
				tank = (float) OptionsDisplay::instance()->getRoamVarianceTank();
			}

			float fvariance = start + ramp - tank;
			if (fvariance < start) fvariance = start;

			unsigned variance = unsigned(fvariance);
			(*patch)->tessalate(variance);
		}

		patch++;
	}
}

void PatchGrid::visibility()
{
	GLCameraFrustum *frustum = GLCameraFrustum::instance();
	int divideWidth = hMap_->getMapWidth() / hMap_->getMapMinWidth();
	int divideHeight = hMap_->getMapHeight() / hMap_->getMapMinHeight();

	// Check visibility
	Vector point, point2;
	Patch **patch = patches_;
	for (int y=0; y<height_; y++)
	{
		for (int x=0; x<width_; x++)
		{
			// Check patch visibility
			point = (*patch)->getMidPoint();
			point2 = (*patch)->getMidPoint();
			point2[2] = MAX(
				hMap_->getMaxHeight(((*patch)->getX() + 2) / divideWidth, 
					((*patch)->getY() + 2)  / divideHeight).asFloat(),
				hMap_->getMaxHeight(((*patch)->getX() + (*patch)->getWidth() - 2) / divideWidth, 
					((*patch)->getY() + (*patch)->getWidth() - 2) / divideHeight ).asFloat()) / 2.0f;
			point[2] = MAX(
				hMap_->getMaxHeight(((*patch)->getX() + 2) / divideWidth, 
					((*patch)->getY() + 2)  / divideHeight).asFloat(),
				hMap_->getMaxHeight(((*patch)->getX() + (*patch)->getWidth() - 2) / divideWidth, 
					((*patch)->getY() + (*patch)->getWidth() - 2) / divideHeight ).asFloat()) / 2.0f;

			float width = MAX(point[2], (*patch)->getWidth() + 5.0f);

			bool pt1 = frustum->sphereInFrustum(point, width,
				GLCameraFrustum::FrustrumGreen);
			bool pt2 = frustum->sphereInFrustum(point2, width,
				GLCameraFrustum::FrustrumGreen);
			(*patch)->getVisible() = (pt1 || pt2);

			patch++;
		}
	}
}

void PatchGrid::draw(PatchSide::DrawType sides)
{
	DIALOG_ASSERT(patches_);
	visibility();
	tesselate();

	glColor3f(1.0f, 1.0f, 1.0f);
	switch(sides)
	{
	case PatchSide::typeTop:
	{
		drawnPatches_ = 0;
		Patch **patch = patches_;
		for (int p=0; p<width_ * height_; p++)
		{			
			if ((*patch)->getVisible())
			{
				drawnPatches_ ++;
				(*patch)->draw(sides);
			}

			patch++;
		}
		GLDynamicVertexArray::instance()->drawROAM();
	}
	break;
	case PatchSide::typeNormals:
	{
		glNormal3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		Patch **patch = patches_;
		for (int p=0; p<width_ * height_; p++)
		{
			if ((*patch)->getVisible())
			{
				(*patch++)->draw(sides);
			}
		}
		glEnd();
	}
	break;
	}
}
