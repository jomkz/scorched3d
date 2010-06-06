////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#if !defined(__INCLUDE_PlacementShadowDefinitionh_INCLUDE__)
#define __INCLUDE_PlacementShadowDefinitionh_INCLUDE__

#include <XML/XMLNode.h>
#include <image/ImageID.h>

class ScorchedContext;
class PlacementShadowDefinition
{
public:
	struct Entry
	{
		Entry(PlacementShadowDefinition *definition,
			FixedVector &position,
			FixedVector &size) : 
			definition_(definition),
			position_(position), size_(size) { }

		PlacementShadowDefinition *definition_;
		FixedVector position_;
		FixedVector size_;
	};

	PlacementShadowDefinition();
	virtual ~PlacementShadowDefinition();

	virtual bool readXML(XMLNode *node);

	void updateLandscapeHeight(ScorchedContext &context,
		FixedVector &position, FixedVector &size);
	void updateLandscapeTexture(bool useShadows,
		ScorchedContext &context,
		FixedVector &position, FixedVector &size);

	void setDrawShadow(bool draw) { drawShadow_ = draw; }

protected:
	bool drawShadow_;
	fixed flattenArea_;
	ImageID groundMap_;
};

#endif // __INCLUDE_PlacementShadowDefinitionh_INCLUDE__
