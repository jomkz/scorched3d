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

#include <GLEXT/GLVertexSetGroup.h>

GLVertexSetGroup::GLVertexSetGroup()
{

}

GLVertexSetGroup::~GLVertexSetGroup()
{

}

void GLVertexSetGroup::draw()
{
	for (unsigned int i=0; i<sets_.size(); i++)
	{
		sets_[i]->draw();
	}
}

void GLVertexSetGroup::destroyGroup()
{
	while (!sets_.empty())
	{
		delete sets_.back();
		sets_.pop_back();		
	}
}

void GLVertexSetGroup::addToGroup(GLVertexSet &set)
{
	sets_.push_back(&set);
}

int GLVertexSetGroup::getNoTris()
{
	int tris = 0;
	for (unsigned int i=0; i<sets_.size(); i++)
	{
		tris += sets_[i]->getNoTris();
	}

	return tris;
}
