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

#if !defined(__INCLUDE_PlacementObjectTargeth_INCLUDE__)
#define __INCLUDE_PlacementObjectTargeth_INCLUDE__

#include <placement/PlacementObject.h>
#include <placement/PlacementGroupDefinition.h>
#include <target/TargetDefinition.h>

class PlacementObjectTarget : public PlacementObject
{
public:
	PlacementObjectTarget();
	virtual ~PlacementObjectTarget();

	virtual bool readXML(XMLNode *node);
	virtual PlacementObject::Type getType() { return PlacementObject::eTarget; }
	virtual void createObject(ScorchedContext &context,
		RandomGenerator &generator,
		unsigned int &playerId,
		PlacementType::Position &position);

protected:
	TargetDefinition targetDef_;
	PlacementGroupDefinition groups_;
};

#endif // __INCLUDE_PlacementObjectTargeth_INCLUDE__
