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

#if !defined(__INCLUDE_VisibilityListh_INCLUDE__)
#define __INCLUDE_VisibilityListh_INCLUDE__

#include <land/LandVisibilityPatch.h>
#include <land/WaterVisibilityPatch.h>
#include <land/TargetVisibilityPatch.h>

template<class T>
class VisibilityList
{
public:
	VisibilityList() : visibleCount_(0), visiblePatches_(0), lastVisiblePatches_(0)
	{

	}
	~VisibilityList()
	{
	}

	void generate(int size)
	{
		visiblePatches_ = lastVisiblePatches_ = new T*[size];
		reset();
	}

	void clear()
	{
		delete [] visiblePatches_;
		visiblePatches_ = 0;
		lastVisiblePatches_ = 0;
		visibleCount_ = 0;
	}

	void reset() 
	{
		visibleCount_ = 0;
		lastVisiblePatches_ = visiblePatches_;
	}

	void add(T *patch)
	{
		*lastVisiblePatches_ = patch;
		visibleCount_++;
		lastVisiblePatches_++;
	}

	int getVisibleCount() { return visibleCount_; }
	T **getVisiblePatches() { return visiblePatches_; }
	T **getLastVisiblePatches() { return lastVisiblePatches_; }

private:
	int visibleCount_;
	T **visiblePatches_, **lastVisiblePatches_;	
};

#endif // __INCLUDE_VisibilityListh_INCLUDE__
