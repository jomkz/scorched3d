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
			XMLEntrySimpleType *entry = ScorchedServer::instance()->
				getOptionsGame().getEntryByName(propertyName_);
			DIALOG_ASSERT(entry && entry->getTypeCatagory() == XMLEntrySimpleType::eSimpleNumberType);
			result_ = atoi(entry->getValueAsString().c_str());
		}

		std::string propertyName_;
		int result_;
	};
	ClientServerOptionsAccessInt callback(propertyName);
	ScorchedServer::getServerThreadCallback().addCallbackSync(callback);
	return callback.result_;
}

std::string ClientServerAccess::getStringProperty(const std::string &propertyName)
{
	class ClientServerOptionsAccessStr : public ThreadCallbackI
	{
	public:
		ClientServerOptionsAccessStr(const std::string &propertyName) : 
			propertyName_(propertyName), result_("")
		{

		}

		virtual void callbackInvoked()
		{
			XMLEntrySimpleType *entry = ScorchedServer::instance()->
				getOptionsGame().getEntryByName(propertyName_);
			DIALOG_ASSERT(entry && entry->getTypeCatagory() == XMLEntrySimpleType::eSimpleStringType);
			result_ = entry->getValueAsString();
		}

		std::string propertyName_;
		std::string result_;
	};
	ClientServerOptionsAccessStr callback(propertyName);
	ScorchedServer::getServerThreadCallback().addCallbackSync(callback);
	return callback.result_;
}
