////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <tanket/TanketTypes.h>
#include <common/Defines.h>

TanketTypes::TanketTypes() : 
	XMLEntryRoot<XMLEntryContainer>(S3D::eModLocation, "data/tanktypes.xml", "tanktypes",
		"TanketTypes", "The set of available tank types"),
	defaultType_(0)
{
	addChildXMLEntry("tanktype", &types_);
}

TanketTypes::~TanketTypes()
{
	clear();
}

bool TanketTypes::loadTanketTypes(ScorchedContext &context)
{
	clear();

	if (!loadFile(true, &context)) return false;
	std::list<TanketType *>::iterator itor = types_.getChildren().begin(),
		end = types_.getChildren().end();
	for (;itor!=end; ++itor)
	{
		if ((*itor)->getUseAsDefault()) defaultType_ = *itor;
	}
	if (!defaultType_)
	{
		defaultType_ = *types_.getChildren().begin();
	}
	return true;
}

TanketType *TanketTypes::getType(const char *name)
{
	std::list<TanketType *>::iterator itor;
	for (itor = types_.getChildren().begin();
		itor != types_.getChildren().end();
		++itor)
	{
		TanketType *type = (*itor);
		if (0 == strcmp(name, type->getName())) return type;
	}
	return 0;
}

void TanketTypes::clear()
{
	defaultType_ = 0;
	types_.clear();
}
