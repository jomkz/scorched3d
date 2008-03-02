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

#if !defined(AFX_PATCHSIDE_H__318589BF_F297_4A61_A052_3D7DFC06178A__INCLUDED_)
#define AFX_PATCHSIDE_H__318589BF_F297_4A61_A052_3D7DFC06178A__INCLUDED_

#include <landscape/Variance.h>
#include <landscape/PatchTexCoord.h>
#include <landscape/TriNodePool.h>

class PatchSide  
{
public:
	PatchSide(HeightMap *hMap, PatchTexCoord *coord, 
		int left, int top, int width);
	virtual ~PatchSide();

	void computeVariance();
	void tesselate(unsigned currentVariance);
	void split(TriNode *node);
	void reset();

	enum DrawType
	{
		typeTop,
		typeNormals
	};

	void draw(DrawType side);

	TriNode *getTriNode();

protected:
	HeightMap *hMap_;
	PatchTexCoord *coord_;
	TriNodePool &triNodePool_;
	int left_, top_, width_;
	TriNode baseTriNode_;
	Variance variance_;

	void recursRender( TriNode *tri, 
						int leftX, int leftY,
						int rightX, int rightY, 
						int apexX, int apexY, 
						int node, DrawType side);

	void recursTessellate( TriNode *tri,
							  int leftX,  int leftY,
							  int rightX, int rightY,
							  int apexX,  int apexY,
							  int node, unsigned currentVariance );
};

#endif // !defined(AFX_PATCHSIDE_H__318589BF_F297_4A61_A052_3D7DFC06178A__INCLUDED_)
