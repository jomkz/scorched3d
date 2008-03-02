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

#include <landscape/SurroundLandscape.h>
#include <landscape/Sky.h>
#include <landscape/Sun.h>
#include <landscape/Landscape.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapemap/LandscapeMaps.h>
#include <GLEXT/GLInfo.h>
#include <GLEXT/GLState.h>
#include <client/ScorchedClient.h>
#include <engine/ScorchedContext.h>
#include <common/Defines.h>
#include <math.h>

SurroundLandscape::SurroundLandscape() : list_(0), tris_(0)
{
}

SurroundLandscape::~SurroundLandscape()
{
}

void SurroundLandscape::generate()
{
	if (list_) glDeleteLists(list_, 1);
	list_ = 0;
	tris_ = 0;
}

void SurroundLandscape::makeNormal(Vector &position, Vector &normal)
{
	LandscapeTex &tex =
		*ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getTex();
	Vector &ambient = tex.skyambience;
	Vector &diffuse = tex.skydiffuse;
	Vector &sunPos = Landscape::instance()->getSky().getSun().getPosition();
	Vector sunDirection = (sunPos - position).Normalize();
	
	float diffuseLight = (((normal.dotP(sunDirection)) / 2.0f) + 0.5f);
	Vector light = diffuse * diffuseLight + ambient;
	light[0] = MIN(1.0f, light[0]);
	light[1] = MIN(1.0f, light[1]);
	light[2] = MIN(1.0f, light[2]);

	float alpha = 1.0f;
	if (position[2] < 1.0f)
	{
		alpha = position[2] / 1.0f;
	}

	glColor4f(light[0], light[1], light[2], alpha);
}

void SurroundLandscape::makeList(bool detail, bool lightMap)
{
	HeightMap &smap = 	
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getSurroundMap();
	int mapWidth =
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getMapWidth();
	int mapHeight =
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getMapHeight();

	float mult = 16.0f;
	int stopWidth = 640 / 16;
	int startWidth = stopWidth + mapWidth / 16;
	int stopHeight = 640 / 16;
	int startHeight = stopHeight + mapHeight / 16;

	Vector offset(-640.0f, -640.0f, -0.1f);
	Vector center(mapWidth / 2.0f, mapHeight / 2.0f, 0.0f);

	glNewList(list_ = glGenLists(1), GL_COMPILE);
	for (int j=0; j<smap.getMapHeight(); j++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int i=0; i<=smap.getMapWidth(); i++)
		{
			if (j>=stopHeight && j<startHeight && i>stopWidth && i<startWidth)
			{
				if (i==stopWidth+1) glEnd();
				if (i==startWidth-1) glBegin(GL_QUAD_STRIP);
			}
			else
			{
				Vector b(
					i * mult + offset[0], 
					(j + 1) * mult + offset[1], 
					smap.getHeight(i, j + 1).asFloat() + offset[2]);
				FixedVector nbf = smap.getNormal(i, j + 1);
				Vector nb(nbf[0].asFloat(), nbf[1].asFloat(), nbf[2].asFloat());
				glNormal3fv(nb);
				if (lightMap) makeNormal(b, nb);
				else glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2f(b[0] / 64.0f, b[1] / 64.0f);  // Tile the tex every 64 pixels
				if (detail) glMultiTexCoord2fARB(GL_TEXTURE2_ARB, b[0] / 64.0f, b[1] / 64.0f);
				glVertex3fv(b);


				Vector a(
					i * mult + offset[0], 
					j * mult + offset[1], 
					smap.getHeight(i, j).asFloat() + offset[2]);
				FixedVector naf = smap.getNormal(i, j);
				Vector na(naf[0].asFloat(), naf[1].asFloat(), naf[2].asFloat());
				glNormal3fv(na);
				if (lightMap) makeNormal(b, na);
				else glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2f(a[0] / 64.0f, a[1] / 64.0f);  // Tile the tex every 64 pixels
				if (detail) glMultiTexCoord2fARB(GL_TEXTURE2_ARB, a[0] / 64.0f, a[1] / 64.0f);
				glVertex3fv(a);

				tris_ += 2;
			}
		}
		glEnd();
	}
	glEndList();
}

void SurroundLandscape::draw(bool detail, bool lightMap)
{
	if (!list_) makeList(detail, lightMap);
	
	GLInfo::addNoTriangles(tris_);
	GLState state(GLState::BLEND_ON);
	Landscape::instance()->getSurroundTexture().draw(true);
	glCallList(list_);
}
