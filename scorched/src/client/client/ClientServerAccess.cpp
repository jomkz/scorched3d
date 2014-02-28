////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <client/ClientServerAccess.h>
#include <engine/ThreadCallback.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>

int ClientServerAccess::getIntProperty(const std::string &propertyName)
{
	class ClientServerOptionsAccessInt : public ThreadCallbackI
	{
	public:
		ClientServerOptionsAccessInt(const std::string &propertyName) : 
			propertyName_(propertyName), result_(-1)
		{

		}

		virtual void callbackInvoked()
		{
			OptionEntry *entry = ScorchedServer::instance()->
				getOptionsGame().getEntryByName(propertyName_);
			DIALOG_ASSERT(entry);
			if (entry->getEntryType() == OptionEntry::OptionEntryIntType)
			{
				OptionEntryInt *intEntry = (OptionEntryInt *) entry;
				result_ = intEntry->getValue();
			}
			else if (entry->getEntryType() == OptionEntry::OptionEntryBoundedIntType)
			{
				OptionEntryBoundedInt *intEntry = (OptionEntryBoundedInt *) entry;
				result_ = intEntry->getValue();
			}
			else DIALOG_ASSERT(entry);
		}

		std::string propertyName_;
		int result_;
	};
	ClientServerOptionsAccessInt callback(propertyName);
	ScorchedServer::getThreadCallback().addCallbackSync(callback);
	return callback.result_;
}

const char *ClientServerAccess::getStringProperty(const std::string &propertyName)
{
	class ClientServerOptionsAccessStr : public ThreadCallbackI
	{
	public:
		ClientServerOptionsAccessStr(const std::string &propertyName) : 
			propertyName_(propertyName), result_(0)
		{

		}

		virtual void callbackInvoked()
		{
			OptionEntry *entry = ScorchedServer::instance()->
				getOptionsGame().getEntryByName(propertyName_);
			DIALOG_ASSERT(entry && entry->getEntryType() == OptionEntry::OptionEntryStringType);
			OptionEntryString *strEntry = (OptionEntryString *) entry;
			result_ = strEntry->getValue();
		}

		std::string propertyName_;
		const char *result_;
	};
	ClientServerOptionsAccessStr callback(propertyName);
	ScorchedServer::getThreadCallback().addCallbackSync(callback);
	return callback.result_;
}
