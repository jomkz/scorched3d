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


#include <landscape/HeightMapRenderer.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLState.h>

void HeightMapRenderer::drawHeightMap(HeightMap &map)
{
		float width = (float) map.getMapWidth();
		float height = (float) map.getMapHeight();

		// Draw the triangles
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);
		int x;
		for (x=0; x<map.getMapWidth()-1; x++)
		{
			for (int y=0; y<map.getMapHeight()-1; y++)
			{
				glTexCoord2f(float(x) / width, float(y) / height);
				glNormal3fv(map.getNormal(x, y).asVector());
				glVertex3f((float)x, (float)y, map.getHeight(x, y).asFloat());
				glTexCoord2f(float(x+1) / width, float(y) / height);
				glNormal3fv(map.getNormal(x + 1, y).asVector());
				glVertex3f((float)x + 1.0f, (float)y, map.getHeight(x + 1, y).asFloat());
				glTexCoord2f(float(x+1) / width, float(y+1) / height);
				glNormal3fv(map.getNormal(x + 1, y + 1).asVector());
				glVertex3f((float)x + 1.0f, (float)y + 1.0f, map.getHeight(x + 1, y + 1).asFloat());

				glTexCoord2f(float(x+1) / width, float(y+1) / height);
				glNormal3fv(map.getNormal(x+1, y+1).asVector());
				glVertex3f((float)x + 1.0f, (float)y + 1.0f, map.getHeight(x + 1, y + 1).asFloat());
				glTexCoord2f(float(x) / width, float(y+1) / height);
				glNormal3fv(map.getNormal(x, y+1).asVector());
				glVertex3f((float)x, (float)y + 1.0f, map.getHeight(x, y + 1).asFloat());
				glTexCoord2f(float(x) / width, float(y) / height);
				glNormal3fv(map.getNormal(x, y).asVector());
				glVertex3f((float)x, (float)y, map.getHeight(x, y).asFloat());
			}
		}
		glEnd();

		if (OptionsDisplay::instance()->getDrawNormals())
		{
			// Draw the normals
			glColor3f(0.0f, 1.0f, 0.0f);
			glBegin(GL_LINES);
			for (x=0; x<=map.getMapWidth(); x++)
			{
				for (int y=0; y<=map.getMapWidth(); y++)
				{
					Vector &Normal = map.getNormal(x, y).asVector();
					Vector Position((float)x, (float)y, map.getHeight(x, y).asFloat());

					glVertex3fv(Position);
					glVertex3fv(Position + Normal * 2.0f);
				}
			}			
			glEnd();
		}
}
