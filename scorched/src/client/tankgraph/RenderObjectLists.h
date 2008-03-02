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

#if !defined(__INCLUDE_RenderObjectListsh_INCLUDE__)
#define __INCLUDE_RenderObjectListsh_INCLUDE__

#include <tankgraph/RenderObjectList.h>

class RenderObjectLists
{
public:
	RenderObjectLists();
	virtual ~RenderObjectLists();

	void reset()
	{
		treeList_.reset();
		modelList_.reset();
		shadowList_.reset();
		twoDList_.reset();
	}

	RenderObjectList &getTreeList() { return treeList_; }
	RenderObjectList &getModelList() { return modelList_; }
	RenderObjectList &getShadowList() { return shadowList_; }
	RenderObjectList &get2DList() { return twoDList_; }

protected:
	RenderObjectList treeList_;
	RenderObjectList modelList_;
	RenderObjectList shadowList_;
	RenderObjectList twoDList_;
};

#endif
