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

#include <land/VisibilityPatchInfos.h>
#include <land/VisibilityPatchQuad.h>
#include <graph/MainCamera.h>
#include <graph/OptionsDisplay.h>

VisibilityPatchInfos::VisibilityPatchInfos() : 
	current_(true), visibilityPatches_(0),
	visibilityThread_(0)
{
}

VisibilityPatchInfos::~VisibilityPatchInfos()
{
}

void VisibilityPatchInfos::generate(int maxLandPatches, int maxWaterPatches,
	VisibilityPatchQuad *visibilityPatches,
	int visibilityWidth, int visibilityHeight)
{
	patchInfos_[0].generate(maxLandPatches, maxWaterPatches);
	patchInfos_[1].generate(maxLandPatches, maxWaterPatches);

	visibilityPatches_ = visibilityPatches;
	visibilityWidth_ = visibilityWidth;
	visibilityHeight_ = visibilityHeight;

	if (!visibilityThread_)
	{
		visibilityThread_ = SDL_CreateThread(
			VisibilityPatchInfos::visibilityThreadFunc, (void *) this);
		visibilityMutex_ = SDL_CreateMutex();
		visibilityCondition_ = SDL_CreateCond();
		DIALOG_ASSERT(visibilityThread_);
	}
}

void VisibilityPatchInfos::startCalculateVisibility()
{
	current_ = !current_;

	if (OptionsDisplay::instance()->getNoThreadedDraw())
	{
		calculateVisibility();
	}
	else
	{
		SDL_CondSignal(visibilityCondition_);
	}
}

int VisibilityPatchInfos::visibilityThreadFunc(void *c)
{
	VisibilityPatchInfos *thi = (VisibilityPatchInfos *) c;
	thi->realVsibilityThreadFunc();
	return 0;
}

void VisibilityPatchInfos::realVsibilityThreadFunc()
{
	SDL_LockMutex(visibilityMutex_);
	for (;;)
	{
		SDL_CondWait(visibilityCondition_, visibilityMutex_);
		calculateVisibility();
	}
	SDL_UnlockMutex(visibilityMutex_);
}

void VisibilityPatchInfos::calculateVisibility()
{
	Vector &cameraPos = 
		MainCamera::instance()->getTarget().getCamera().getCurrentPos();

	// NOTE: We've swapped the 1 and 0
	// so we calculate the one that is NOT current
	VisibilityPatchInfo &patchInfo = patchInfos_[current_?1:0];
	patchInfo.reset();

	// Calculate visibility
	VisibilityPatchQuad *currentPatch = visibilityPatches_;
	for (int y=0; y<visibilityHeight_; y++)
	{
		for (int x=0; x<visibilityWidth_; x++, currentPatch++)
		{
			currentPatch->calculateVisibility(patchInfo, cameraPos);
		}
	}
}

void VisibilityPatchInfos::endCalculateVisibility()
{
	SDL_LockMutex(visibilityMutex_);
	SDL_UnlockMutex(visibilityMutex_);
}
