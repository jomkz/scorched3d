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
	waterVisibilityPatch_(0)
{
}

VisibilityPatchQuad::~VisibilityPatchQuad()
{
	delete topLeft_; delete topRight_;
	delete botLeft_; delete botRight_;
}

void VisibilityPatchQuad::setLocation(VisibilityPatchGrid *patchGrid, int x, int y, int size)
{
	x_ = x; y_ = y; size_ = size;
	position_ = Vector(x_ + size_ / 2, y_ + size_ / 2);

	if (size > 32)
	{
		topLeft_ = new VisibilityPatchQuad();
		topRight_ = new VisibilityPatchQuad();
		botLeft_ = new VisibilityPatchQuad();
		botRight_ = new VisibilityPatchQuad();

		topLeft_->setLocation(patchGrid, x, y, size / 2);
		topRight_->setLocation(patchGrid, x + size / 2, y, size / 2);
		botLeft_->setLocation(patchGrid, x, y + size / 2, size / 2);
		botRight_->setLocation(patchGrid, x + size / 2, y + size / 2, size / 2);
	}

	if (size == 32)
	{
		// Land
		landVisibilityPatch_ = patchGrid->getLandVisibilityPatch(x, y);
	}
	else if (size == 128)
	{
		// Water
		waterVisibilityPatch_ = patchGrid->getWaterVisibilityPatch(x, y);
	}
}

void VisibilityPatchQuad::setNotVisible(Vector &cameraPos)
{
	if (landVisibilityPatch_) landVisibilityPatch_->setVisible(cameraPos, false);
	if (waterVisibilityPatch_) waterVisibilityPatch_->setVisible(cameraPos, false);

	// Update Children
	if (topLeft_) topLeft_->setNotVisible(cameraPos);
	if (topRight_) topRight_->setNotVisible(cameraPos);
	if (botLeft_) botLeft_->setNotVisible(cameraPos);
	if (botRight_) botRight_->setNotVisible(cameraPos);
}

void VisibilityPatchQuad::setVisible(Vector &cameraPos)
{
	if (landVisibilityPatch_) landVisibilityPatch_->setVisible(cameraPos, true);
	if (waterVisibilityPatch_) waterVisibilityPatch_->setVisible(cameraPos, true);

	// Update Children
	if (topLeft_) topLeft_->calculateVisibility(cameraPos);
	if (topRight_) topRight_->calculateVisibility(cameraPos);
	if (botLeft_) botLeft_->calculateVisibility(cameraPos);
	if (botRight_) botRight_->calculateVisibility(cameraPos);	
}

void VisibilityPatchQuad::calculateVisibility(Vector &cameraPos)
{
	if (!GLCameraFrustum::instance()->sphereInFrustum(position_, float(size_)))
	{
		setNotVisible(cameraPos);
	}
	else
	{
		setVisible(cameraPos);
	}
}
