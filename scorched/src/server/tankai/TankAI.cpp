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

#include <tankai/TankAI.h>
#include <lang/LangResource.h>
#include <XML/XMLNode.h>

bool TankAI::tankAILogging_ = false;

TankAI::TankAI(const char *typeName, const char *description) : 
	XMLEntryContainer(typeName, description),
	name_("The human readable name given to this tank ai"),
	description_("The description of this tank ai"),
	availableForRandom_("If this tank ai can be chosen when the player does not explicitly select an AI but instead chooses a random AI", 0, true),
	availableForPlayers_("If this tank ai is available for fake players/bots to use (or if it is only usable by things like automated turrets)", 0, true)
{
	addChildXMLEntry("name", &name_, "description", &description_);
	addChildXMLEntry("availableforrandom", &availableForRandom_);
	addChildXMLEntry("availableforplayers", &availableForPlayers_);
}

TankAI::~TankAI()
{
}

ToolTip *TankAI::getToolTip()
{
	toolTip_.setText(ToolTip::ToolTipInfo, LANG_STRING(name_.getValue()), 
		LANG_RESOURCE(name_.getValue() + "_ai_description",  description_.getValue()));
	return &toolTip_;
}
