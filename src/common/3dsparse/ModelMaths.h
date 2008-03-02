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

#if !defined(__INCLUDE_ModelMathsh_INCLUDE__)
#define __INCLUDE_ModelMathsh_INCLUDE__

#include <common/Vector.h>

class ModelMaths
{
public:
	static void angleQuaternion(const Vector &angles, 
		float quaternion[4]);
	static void quaternionSlerp(float p[4], 
		float q[4], float t, float qt[4]);

	static void angleMatrix(
		const Vector &angles, float matrix[3][4]);
	static void quaternionMatrix(
		float quaternion[4], float matrix[3][4]);

	static void concatTransforms(const float in1[3][4],
		const float in2[3][4], float out[3][4]);
	static void vectorIRotate(const Vector &in1, 
		const float in2[3][4], Vector &out);
	static float dotProduct(const Vector &v1, 
		const Vector &v2)
	{
		return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
	}
	static void vectorRotate(const Vector &in1, 
		const float in2[3][4], Vector &out)
	{
		out[0] = dotProduct(in1, in2[0]);
		out[1] = dotProduct(in1, in2[1]);
		out[2] = dotProduct(in1, in2[2]);
	}

private:
	ModelMaths();
};

#endif // __INCLUDE_ModelMathsh_INCLUDE__
