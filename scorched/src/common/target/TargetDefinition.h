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

#if !defined(__INCLUDE_TargetDefinitionh_INCLUDE__)
#define __INCLUDE_TargetDefinitionh_INCLUDE__

#include <common/ModelID.h>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>
#include <placement/PlacementShadowDefinition.h>
#include <placement/PlacementGroupDefinition.h>

class Target;
class TargetDefinition
{
public:
	TargetDefinition();
	virtual ~TargetDefinition();

	virtual bool readXML(XMLNode *node, const char *base);
	Target *createTarget(unsigned int playerId,
		FixedVector &position,
		FixedVector &velocity,
		ScorchedContext &context,
		RandomGenerator &generator);

	PlacementShadowDefinition &getShadow() { return shadow_; }
	PlacementGroupDefinition &getGroups() { return groups_; }

protected:
	fixed life_;
	bool boundingsphere_;
	bool driveovertodestroy_, flattendestroy_;
	bool nocollision_, nodamageburn_, nofalling_;
	bool displaydamage_, displayshadow_;
	bool displayhardwareshadow_;
	bool nofallingdamage_;
	LangString name_;
	std::string parachute_;
	std::string shield_;
	FixedVector size_;
	fixed modelscale_, modelscalediff_;
	fixed modelrotation_, modelbrightness_;
	fixed modelrotationsnap_;
	fixed border_;
	std::string removeaction_;
	std::string burnaction_;
	ModelID modelId_;
	ModelID modelburntId_;
	PlacementShadowDefinition shadow_;
	PlacementGroupDefinition groups_;
};

#endif // __INCLUDE_TargetDefinitionh_INCLUDE__
