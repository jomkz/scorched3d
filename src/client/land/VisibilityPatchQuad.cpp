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

#include <land/VisibilityPatchQuad.h>
#include <land/VisibilityPatchGrid.h>
#include <GLEXT/GLCameraFrustum.h>

VisibilityPatchQuad::VisibilityPatchQuad() :
	topLeft_(0), topRight_(0),
	botLeft_(0), botRight_(0),
	landVisibilityPatch_(0),
	waterVisibilityPatch_(0),
	targetVisibilityPatch_(0)
{
}

VisibilityPatchQuad::~VisibilityPatchQuad()
{
	delete topLeft_; delete topRight_;
	delete botLeft_; delete botRight_;
}

void VisibilityPatchQuad::setLocation(VisibilityPatchGrid *patchGrid, int x, int y, int size,
	int mapwidth, int mapheight)
{
	x_ = x; y_ = y; size_ = size;
	position_ = Vector(x_ + size_ / 2, y_ + size_ / 2);

	if (size > 32)
	{
		topLeft_ = new VisibilityPatchQuad();
		topRight_ = new VisibilityPatchQuad();
		botLeft_ = new VisibilityPatchQuad();
		botRight_ = new VisibilityPatchQuad();

		topLeft_->setLocation(patchGrid, x, y, size / 2, mapwidth, mapheight);
		topRight_->setLocation(patchGrid, x + size / 2, y, size / 2, mapwidth, mapheight);
		botLeft_->setLocation(patchGrid, x, y + size / 2, size / 2, mapwidth, mapheight);
		botRight_->setLocation(patchGrid, x + size / 2, y + size / 2, size / 2, mapwidth, mapheight);
	}

	if (size == 32)
	{
		if ((x + size_) > 0 && 
			(y + size_) > 0 &&
			x < mapwidth && y < mapheight) 
		{
			// Land
			landVisibilityPatch_ = patchGrid->getLandVisibilityPatch(x, y);
		}
		// Targets
		targetVisibilityPatch_ = patchGrid->getTargetVisibilityPatch(x, y);
	}
	else if (size == 128)
	{
		// Water
		waterVisibilityPatch_ = patchGrid->getWaterVisibilityPatch(x, y);
	}
}

void VisibilityPatchQuad::setNotVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos)
{
	if (landVisibilityPatch_) landVisibilityPatch_->setNotVisible();
	if (targetVisibilityPatch_) targetVisibilityPatch_->setNotVisible();
	if (waterVisibilityPatch_) waterVisibilityPatch_->setNotVisible();

	// Update Children
	if (topLeft_) topLeft_->setNotVisible(patchInfo, cameraPos);
	if (topRight_) topRight_->setNotVisible(patchInfo, cameraPos);
	if (botLeft_) botLeft_->setNotVisible(patchInfo, cameraPos);
	if (botRight_) botRight_->setNotVisible(patchInfo, cameraPos);
}

void VisibilityPatchQuad::setVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos)
{
	if (landVisibilityPatch_)
	{
		float distance = (cameraPos - landVisibilityPatch_->getPosition()).Magnitude();
		if (landVisibilityPatch_->setVisible(distance))
		{
			patchInfo.getLandVisibility().add(landVisibilityPatch_);
		}
	}
	if (targetVisibilityPatch_)
	{
		if (targetVisibilityPatch_->hasTargets() ||
			targetVisibilityPatch_->hasTrees())
		{
			float distance = (cameraPos - targetVisibilityPatch_->getPosition()).Magnitude();
			if (targetVisibilityPatch_->setVisible(distance))
			{
				if (targetVisibilityPatch_->hasTargets()) 
					patchInfo.getTargetVisibility().add(targetVisibilityPatch_);
				if (targetVisibilityPatch_->hasTrees()) 
					patchInfo.getTreeVisibility().add(targetVisibilityPatch_);
			}
		}
	}

	if (waterVisibilityPatch_)
	{
		if (waterVisibilityPatch_->setVisible(cameraPos))
		{
			patchInfo.getWaterVisibility(waterVisibilityPatch_->getPatchIndex()).add(waterVisibilityPatch_);
		}
	}

	// Update Children
	if (topLeft_) topLeft_->calculateVisibility(patchInfo, cameraPos);
	if (topRight_) topRight_->calculateVisibility(patchInfo, cameraPos);
	if (botLeft_) botLeft_->calculateVisibility(patchInfo, cameraPos);
	if (botRight_) botRight_->calculateVisibility(patchInfo, cameraPos);	
}

void VisibilityPatchQuad::calculateVisibility(VisibilityPatchInfo &patchInfo, Vector &cameraPos)
{
	Vector *position = &position_;
	float size = float(size_);
	if (landVisibilityPatch_)
	{
		position = &landVisibilityPatch_->getPosition();
		size = landVisibilityPatch_->getBoundingSize();
	}

	bool visible = true;
	if (size_ == 32 || size_ == 128 || size_ == 512)
	{
		visible = GLCameraFrustum::instance()->sphereInFrustum(*position, size);
	}

	if (!visible)
	{
		setNotVisible(patchInfo, cameraPos);
	}
	else
	{
		setVisible(patchInfo, cameraPos);
	}
}
