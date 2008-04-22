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

#if !defined(__INCLUDE_VisibilityPatchInfosh_INCLUDE__)
#define __INCLUDE_VisibilityPatchInfosh_INCLUDE__

#include <land/VisibilityPatchInfo.h>
#include <SDL/SDL_thread.h>

class VisibilityPatchQuad;
class VisibilityPatchInfos
{
public:
	VisibilityPatchInfos();
	~VisibilityPatchInfos();

	VisibilityPatchInfo &getCurrent() {
		return patchInfo_; }

	void generate(int maxLandPatches, int maxWaterPatches,
		VisibilityPatchQuad *visibilityPatches,
		int visibilityWidth, int visibilityHeight);
	void calculateVisibility();
	void startCalculateVisibility();
	void endCalculateVisibility();

protected:
	SDL_Thread *visibilityThread_;
	SDL_mutex *visibilityMutex_;
	SDL_cond *visibilityCondition_;
	VisibilityPatchQuad *visibilityPatches_;
	int visibilityWidth_, visibilityHeight_;
	VisibilityPatchInfo patchInfo_;
	bool current_;

	static int visibilityThreadFunc(void *c);
	void realVsibilityThreadFunc();
};

#endif // __INCLUDE_VisibilityPatchInfosh_INCLUDE__
