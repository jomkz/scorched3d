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

#include <land/LandVisibilityPatchQuad.h>
#include <land/LandVisibilityPatchGrid.h>
#include <GLEXT/GLCameraFrustum.h>

LandVisibilityPatchQuad::LandVisibilityPatchQuad() :
	topLeft_(0), topRight_(0),
	botLeft_(0), botRight_(0),
	LandVisibilityPatch_(0)
{
}

LandVisibilityPatchQuad::~LandVisibilityPatchQuad()
{
	delete topLeft_; delete topRight_;
	delete botLeft_; delete botRight_;
}

void LandVisibilityPatchQuad::setLocation(LandVisibilityPatchGrid *patchGrid, int x, int y, int size)
{
	x_ = x; y_ = y; size_ = size;
	position_ = Vector(x_ + size_ / 2, y_ + size_ / 2);

	if (size > 64)
	{
		topLeft_ = new LandVisibilityPatchQuad();
		topRight_ = new LandVisibilityPatchQuad();
		botLeft_ = new LandVisibilityPatchQuad();
		botRight_ = new LandVisibilityPatchQuad();

		topLeft_->setLocation(patchGrid, x, y, size / 2);
		topRight_->setLocation(patchGrid, x + size / 2, y, size / 2);
		botLeft_->setLocation(patchGrid, x, y + size / 2, size / 2);
		botRight_->setLocation(patchGrid, x + size / 2, y + size / 2, size / 2);
	}

	if (size == 64)
	{
		// Land
		LandVisibilityPatch_ = patchGrid->getLandVisibilityPatch(x, y);
	}
	else if (size == 128)
	{
		// Water
	}
}

void LandVisibilityPatchQuad::setNotVisible()
{
	if (LandVisibilityPatch_) LandVisibilityPatch_->setVisible(false);

	// Update Children
	if (topLeft_) topLeft_->setNotVisible();
	if (topRight_) topRight_->setNotVisible();
	if (botLeft_) botLeft_->setNotVisible();
	if (botRight_) botRight_->setNotVisible();
}

void LandVisibilityPatchQuad::setVisible()
{
	if (LandVisibilityPatch_) LandVisibilityPatch_->setVisible(true);

	// Update Children
	if (topLeft_) topLeft_->calculateVisibility();
	if (topRight_) topRight_->calculateVisibility();
	if (botLeft_) botLeft_->calculateVisibility();
	if (botRight_) botRight_->calculateVisibility();	
}

void LandVisibilityPatchQuad::calculateVisibility()
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
