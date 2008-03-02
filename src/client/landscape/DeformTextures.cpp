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

#include <landscape/DeformTextures.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <GLEXT/GLImageModifier.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLStateExtension.h>
#include <common/Defines.h>
#include <math.h>

void DeformTextures::deformLandscape(Vector &pos, float radius, 
	Image &scorchedMap, 
	DeformLandscape::DeformPoints &map)
{
	HeightMap &hmap = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap();
	int iradius = (int) radius + 1;
	if (iradius > 49) iradius = 49;

	float pixelsPerSW = (float)(Landscape::instance()->getMainMap().getWidth()) / float(hmap.getMapWidth());
	float pixelsPerSH = (float)(Landscape::instance()->getMainMap().getHeight()) / float(hmap.getMapHeight());

	GLint x = GLint((pos[0] - radius) * pixelsPerSW);
	GLint y = GLint((pos[1] - radius) * pixelsPerSH);
	GLsizei w = GLsizei(pixelsPerSW * 2.0f * radius);
	GLsizei h = GLsizei(pixelsPerSH * 2.0f * radius);

	x = MAX(x, 0);
	y = MAX(y, 0);
	w = MIN(w, Landscape::instance()->getMainMap().getWidth() - x);
	h = MIN(h, Landscape::instance()->getMainMap().getHeight() - y);

	if (!GLStateExtension::getNoTexSubImage() &&
		x < Landscape::instance()->getMainMap().getWidth() &&
		y < Landscape::instance()->getMainMap().getHeight() && 
		x + w < Landscape::instance()->getMainMap().getWidth() &&
		y + h < Landscape::instance()->getMainMap().getHeight())
	{
		int landscapeWidth = Landscape::instance()->getMainMap().getWidth();
		int width = 3 * landscapeWidth;
		width   = (width + 3) & ~3;	

		GLubyte *bytes = 
			Landscape::instance()->getMainMap().getBits() + ((width * y) + x * 3);
		GLubyte *destBits = bytes;
		for (int b=0; b<h;b++)
		{
			float mapYf = float(b) / pixelsPerSH + 2.0f;
			float mapYb = mapYf - floorf(mapYf);
			float mapYa = 1.0f - mapYb;
            int mapY = int(mapYf);

			for (int a=0; a<w; a++)
			{
				float mapXf = float(a) / pixelsPerSW + 2.0f;
				float mapXb = mapXf - floorf(mapXf);
				float mapXa = 1.0f - mapXb;
				int mapX = int(mapXf);

				if (mapX < 99 && mapY < 99)
				{
					float mag = 0.0f;
					if (a < w-1 && b < h-1)
					{
						float maga = 
							map.map[mapX][mapY].asFloat() * mapXa +
							map.map[mapX + 1][mapY].asFloat() * mapXb;
						float magb = 
							map.map[mapX][mapY + 1].asFloat() * mapXa +
							map.map[mapX + 1][mapY + 1].asFloat() * mapXb;
						mag = maga * mapYa + magb * mapYb;
					}
					else 
					{
						mag = map.map[mapX][mapY].asFloat();
					}
					
					if (mag > 0.0f)
					{
						int posX = (x + a) % scorchedMap.getWidth();
						int posY = (y + b) % scorchedMap.getHeight();
						GLubyte *srcBits = 
							scorchedMap.getBits() + ((scorchedMap.getWidth() * posY * 3) + posX * 3);

						destBits[0] = (GLubyte) ((float(srcBits[0]) * mag) + 
							(float(destBits[0]) * (1.0f - mag)));
						destBits[1] = (GLubyte) ((float(srcBits[1]) * mag) + 
							(float(destBits[1]) * (1.0f - mag)));
						destBits[2] = (GLubyte) ((float(srcBits[2]) * mag) + 
							(float(destBits[2]) * (1.0f - mag)));
					}
				}
				destBits +=3;
			}
			destBits += width - w * 3;
		}

		GLState currentState(GLState::TEXTURE_ON);
		Landscape::instance()->getMainTexture().draw(true);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, landscapeWidth);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 
						x, y, 
						w, h, 
						GL_RGB, GL_UNSIGNED_BYTE, 
						bytes);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	}
}
