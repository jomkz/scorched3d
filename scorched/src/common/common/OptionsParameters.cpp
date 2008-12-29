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

#include <common/OptionsParameters.h>
#include <common/Defines.h>

OptionsParameters::OptionsParameters() :
	settingsdir_(options_, "settingsdir",
		"The directory in the users home directory that the scorched3d settings are stored in.", 0, ".scorched3d"),
	allowexceptions_(options_, "allowexceptions",
		"Allows any program exceptions to be thrown (core dumps)", 0, false)
{

}

OptionsParameters::~OptionsParameters()
{
	
}

std::list<OptionEntry *> &OptionsParameters::getOptions()
{
	return options_;
}

std::list<OptionEntry *> &OptionsParameters::getNonParamOptions()
{
	return nonParamOptions_;
}

