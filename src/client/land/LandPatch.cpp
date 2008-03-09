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

#include <land/LandPatch.h>
#include <GLEXT/GLState.h>

LandPatch::LandPatch() : visible_(false)
{
}

LandPatch::~LandPatch()
{
}

void LandPatch::draw()
{
	GLState currentState(GLState::TEXTURE_OFF);

	glLineWidth(2.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
		glVertex2f(location_[0], location_[1]);
		glVertex2f(location_[0] + 64.0f, location_[1]);
		glVertex2f(location_[0] + 64.0f, location_[1] + 64.0f);
		glVertex2f(location_[0], location_[1] + 64.0f);
	glEnd();
	glLineWidth(1.0);
}
