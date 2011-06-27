////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////


#include <common/VectorLib.h>
#include <common/Defines.h>
#include <math.h>

Vector VectorLib::reflection(Vector &ray, Vector &normal)
{
	Vector s = ray.Normalize();
	Vector n = normal.Normalize();

	return -((n * (2 * s.dotP(n))) - s);
}

bool VectorLib::sphereIntersection(Vector &start, Vector &end, 
									Vector &sphereCentre, float radius, float &distance)
{
	if (start == end) return false;

	Vector EO = sphereCentre - start;
	Vector V = (end - start).Normalize();
	float v = EO.dotP(V);
	float disc = (radius * radius) - (EO.dotP(EO) - (v * v));

	if (disc >= 0.0f)
	{
		float d = sqrtf(disc);
		if (v - d > 0)
		{
			distance = v - d;
			return (distance >= 0.0f && distance <= 1.0f);
		}
		else if (v + d > 0)
		{
			distance = v + d;
			return (distance >= 0.0f && distance <= 1.0f);
		}
	}

	return false;
}
