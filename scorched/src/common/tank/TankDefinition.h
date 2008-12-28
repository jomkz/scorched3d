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

#if !defined(__INCLUDE_TankDefinitionh_INCLUDE__)
#define __INCLUDE_TankDefinitionh_INCLUDE__

#include <common/ModelID.h>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>
#include <placement/PlacementShadowDefinition.h>
#include <placement/PlacementGroupDefinition.h>

class Tank;
class TankDefinition
{
public:
	TankDefinition();
	virtual ~TankDefinition();

	virtual bool readXML(XMLNode *node, const char *base);
	Tank *createTank(unsigned int playerId,
		FixedVector &position,
		ScorchedContext &context,
		RandomGenerator &generator);

	FixedVector &getSize() { return size_; }
	PlacementShadowDefinition &getShadow() { return shadow_; }
	PlacementGroupDefinition &getGroups() { return groups_; }

protected:
	fixed life_;
	bool boundingsphere_;
	bool driveovertodestroy_;
	LangString name_;
	std::string parachute_;
	std::string shield_;
	FixedVector size_;
	std::string removeaction_;
	std::string burnaction_;
	PlacementShadowDefinition shadow_;
	PlacementGroupDefinition groups_;

	int team_;
	std::string ai_;
	std::string tankmodel_;

};

#endif // __INCLUDE_TankDefinitionh_INCLUDE__
