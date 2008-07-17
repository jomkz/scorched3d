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

#include <graph/OptionsDisplayConsole.h>
#include <graph/OptionsDisplay.h>
#include <common/OptionsScorched.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>

OptionsDisplayConsole *OptionsDisplayConsole::instance_ = 0;

OptionsDisplayConsole *OptionsDisplayConsole::instance()
{
	if (!instance_)
	{
		instance_ = new OptionsDisplayConsole;
	}

	return instance_;
}

OptionsDisplayConsole::OptionsDisplayConsole()

{
}

OptionsDisplayConsole::~OptionsDisplayConsole()
{	
}

void OptionsDisplayConsole::addDisplayToConsole()
{
	// remove existing adapters
	while (!adapters_.empty())
	{
		ConsoleRuleFnIOptionsAdapter *adapter = adapters_.front();
		adapters_.pop_front();
		delete adapter;
	}

	// Add all of the options to the console
	std::list<OptionEntry *>::iterator itor;
	for (itor = OptionsDisplay::instance()->getOptions().begin();
		itor != OptionsDisplay::instance()->getOptions().end();
		itor++)
	{
		OptionEntry *entry = (*itor);
		if ((entry->getData() & OptionsDisplay::DebugOnly) &&
			!ScorchedClient::instance()->getOptionsGame().getDebugFeatures() &&
			ClientParams::instance()->getConnectedToServer())
		{
			entry->setValueFromString(entry->getDefaultValueAsString());
		}
		else if (!(entry->getData() & OptionEntry::DataDepricated))
		{
			bool write = false;
			if (entry->getData() & OptionsDisplay::RWAccess) write = true;

			adapters_.push_back(new ConsoleRuleFnIOptionsAdapter(
				*entry,
				write));
		}
	}
}

