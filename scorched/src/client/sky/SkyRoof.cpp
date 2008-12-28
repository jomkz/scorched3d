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

#include <sky/SkyRoof.h>
#include <sky/Sky.h>
#include <sky/Sun.h>
#include <landscape/Landscape.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <engine/ScorchedContext.h>
#include <GLEXT/GLInfo.h>
#include <common/Defines.h>
#include <math.h>

SkyRoof::SkyRoof() : list_(0), tris_(0)
{
}

SkyRoof::~SkyRoof()
{
}

void SkyRoof::generate()
{
	if (list_) glDeleteLists(list_, 1);
	list_ = 0;
	tris_ = 0;
}

void SkyRoof::makeNormal(Vector &position, Vector &normal)
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

	glColor3fv(light);
}

void SkyRoof::makeList()
{	
	HeightMap &rmap = ScorchedClient::instance()->
		getLandscapeMaps().getRoofMaps().getRoofMap();
	HeightMap &hmap = ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getHeightMap();

	float multWidth = float(hmap.getMapWidth()) / float(rmap.getMapWidth());
	float multHeight = float(hmap.getMapHeight()) / float(rmap.getMapHeight());
	
	glNewList(list_ = glGenLists(1), GL_COMPILE);
		for (int j=0; j<rmap.getMapHeight(); j++)
		{
			glBegin(GL_QUAD_STRIP);
			for (int i=0; i<=rmap.getMapWidth(); i++)
			{
				Vector a(i * multWidth, j * multHeight, rmap.getHeight(i, j).asFloat());
				makeNormal(a, rmap.getNormal(i, j).asVector());
				glTexCoord2f(a[0] / float(rmap.getMapWidth()), a[1] / float(rmap.getMapHeight()));
				glVertex3fv(a);
				
				Vector b(i * multWidth, (j + 1) * multHeight, rmap.getHeight(i, j + 1).asFloat());
				makeNormal(b, rmap.getNormal(i, j + 1).asVector());
				glTexCoord2f(b[0] / float(rmap.getMapWidth()), b[1] / float(rmap.getMapHeight()));
				glVertex3fv(b);

				tris_ += 2;
			}
			glEnd();
		}

		for (int i=0; i<rmap.getMapWidth(); i++)
		{
			{
				Vector a(i * multWidth, 0.0f, rmap.getHeight(i, 0).asFloat());
				Vector na = rmap.getNormal(i,0).asVector();
				Vector b((i + 1) * multWidth, 0.0f, rmap.getHeight(i + 1, 0).asFloat());
				Vector nb = rmap.getNormal(i + 1, 0).asVector();
				drawSegment(a, b, na, nb);
			}
			{
				Vector b(i * multWidth, (float) hmap.getMapHeight(), 
					rmap.getHeight(i, rmap.getMapHeight()).asFloat());
				Vector nb = rmap.getNormal(i, rmap.getMapHeight()).asVector();
				Vector a((i + 1) * multWidth, (float) hmap.getMapHeight(), 
					rmap.getHeight(i + 1, rmap.getMapHeight()).asFloat());
				Vector na = rmap.getNormal(i + 1, rmap.getMapHeight()).asVector();
				drawSegment(a, b, na, nb);
			}
		}
		for (int i=0; i<rmap.getMapHeight(); i++)
		{
			{
				Vector b(0.0f, i * multHeight, rmap.getHeight(0, i).asFloat());
				Vector nb = rmap.getNormal(0, i).asVector();
				Vector a(0.0f, (i + 1) * multHeight, rmap.getHeight(0, i + 1).asFloat());
				Vector na = rmap.getNormal(0, i + 1).asVector();
				drawSegment(a, b, na, nb);
			}
			{
				Vector a((float) hmap.getMapWidth(), i * multHeight, 
					rmap.getHeight(rmap.getMapWidth(), i).asFloat());
				Vector na = rmap.getNormal(rmap.getMapWidth(), i).asVector();
				Vector b((float) hmap.getMapWidth(), (i + 1) * multHeight, 
					rmap.getHeight(rmap.getMapWidth(), i + 1).asFloat());
				Vector nb = rmap.getNormal(rmap.getMapWidth(), i + 1).asVector();
				drawSegment(a, b, na, nb);
			}
		}

	glEndList();
}

void SkyRoof::drawSegment(Vector &a, Vector &b, Vector &na, Vector &nb)
{
	HeightMap &rmap = ScorchedClient::instance()->
		getLandscapeMaps().getRoofMaps().getRoofMap();
	HeightMap &hmap = ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getHeightMap();
	LandscapeDefn &defn =
		*ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getDefn();
	LandscapeDefnRoofCavern *cavern = 
		(LandscapeDefnRoofCavern *) defn.roof;
	float hemispehereRadius = cavern->width.asFloat();

	const int steps = 5;

	float heighta = a[2];
	float heightb = b[2];
	Vector midPointA(float(hmap.getMapWidth() / 2), float(hmap.getMapHeight() / 2), heighta);
	Vector midPointB(float(hmap.getMapWidth() / 2), float(hmap.getMapHeight() / 2), heightb);

	Vector diffa = (a - midPointA);
	Vector diffb = (b - midPointB);
	float dista = diffa.Magnitude();
	float distb = diffb.Magnitude();
	diffa.StoreNormalize();
	diffb.StoreNormalize();
	diffa *= (hemispehereRadius - dista) / float(steps);
	diffb *= (hemispehereRadius - distb) / float(steps);

	glBegin(GL_QUAD_STRIP);
	for (int i=0; i<=steps + 3; i++)
	{
		{
			Vector c = a + diffa;
			Vector e = (a - c).Normalize();
			Vector f = (a - b).Normalize();
			Vector n = e * f;

			glTexCoord2f(a[0] / float(hmap.getMapWidth()), a[1] / float(hmap.getMapHeight()));
			if (i < 1) n = na;
			makeNormal(a, n);
			glVertex3fv(a);
		}
		
		{
			Vector c = b + diffb;
			Vector f = (b - c).Normalize();
			Vector e = (b - a).Normalize();
			Vector n = e * f;

			glTexCoord2f(b[0] / float(hmap.getMapWidth()), b[1] / float(hmap.getMapHeight()));
			if (i < 1) n = nb;
			makeNormal(b, n);
			glVertex3fv(b);
		}

		a += diffa;
		b += diffb;
		a[2] = heighta * cosf(1.57f * float(i) / float(steps));
		b[2] = heightb * cosf(1.57f * float(i) / float(steps));

		tris_ += 2;
	}	
	glEnd();
}

void SkyRoof::draw()
{
	if (!list_) makeList();
	
	GLInfo::addNoTriangles(tris_);
	Landscape::instance()->getRoofTexture().draw(true);
	glCallList(list_);
}
