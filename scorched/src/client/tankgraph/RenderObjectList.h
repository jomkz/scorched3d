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

#if !defined(__INCLUDE_RenderObjectListh_INCLUDE__)
#define __INCLUDE_RenderObjectListh_INCLUDE__

#include <tankgraph/RenderObject.h>

class RenderObjectList
{
public:
	RenderObjectList();
	virtual ~RenderObjectList();

	RenderObject **getObjects() { return objects_; }
	unsigned int getCount() { return count_; }

	void reset() { count_ = 0; current_ = objects_; }
	void add(RenderObject *object);

protected:
	RenderObject **objects_;
	RenderObject **current_;
	unsigned int count_;
	unsigned int capacity_;

private:
	RenderObjectList(RenderObjectList &);
	const RenderObjectList & operator=(const RenderObjectList &);
};

#endif
