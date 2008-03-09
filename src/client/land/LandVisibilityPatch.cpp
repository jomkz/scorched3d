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
#include <land/LandPatchGrid.h>
#include <GLEXT/GLCameraFrustum.h>

LandVisibilityPatch::LandVisibilityPatch() :
	topLeft_(0), topRight_(0),
	botLeft_(0), botRight_(0),
	landPatch_(0)
{
}

LandVisibilityPatch::~LandVisibilityPatch()
{
	delete topLeft_; delete topRight_;
	delete botLeft_; delete botRight_;
}

void LandVisibilityPatch::setLocation(LandPatchGrid *patchGrid, int x, int y, int size)
{
	x_ = x; y_ = y; size_ = size;
	position_ = Vector(x_ + size_ / 2, y_ + size_ / 2);

	if (size > 64)
	{
		topLeft_ = new LandVisibilityPatch();
		topRight_ = new LandVisibilityPatch();
		botLeft_ = new LandVisibilityPatch();
		botRight_ = new LandVisibilityPatch();

		topLeft_->setLocation(patchGrid, x, y, size / 2);
		topRight_->setLocation(patchGrid, x + size / 2, y, size / 2);
		botLeft_->setLocation(patchGrid, x, y + size / 2, size / 2);
		botRight_->setLocation(patchGrid, x + size / 2, y + size / 2, size / 2);
	}

	if (size == 64)
	{
		// Land
		landPatch_ = patchGrid->getLandPatch(x, y);
	}
	else if (size == 128)
	{
		// Water
	}
}

void LandVisibilityPatch::setNotVisible()
{
	if (landPatch_) landPatch_->setVisible(false);

	// Update Children
	if (topLeft_) topLeft_->setNotVisible();
	if (topRight_) topRight_->setNotVisible();
	if (botLeft_) botLeft_->setNotVisible();
	if (botRight_) botRight_->setNotVisible();
}

void LandVisibilityPatch::setVisible()
{
	if (landPatch_) landPatch_->setVisible(true);

	// Update Children
	if (topLeft_) topLeft_->calculateVisibility();
	if (topRight_) topRight_->calculateVisibility();
	if (botLeft_) botLeft_->calculateVisibility();
	if (botRight_) botRight_->calculateVisibility();	
}

void LandVisibilityPatch::calculateVisibility()
{
	if (!GLCameraFrustum::instance()->sphereInFrustum(position_, float(size_)))
	{
		setNotVisible();
	}
	else
	{
		setVisible();
	}
}
