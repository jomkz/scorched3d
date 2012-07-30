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

#if !defined(__INCLUDE_RocketWindowStateh_INCLUDE__)
#define __INCLUDE_RocketWindowStateh_INCLUDE__

#include <string>
#include <list>
#include <map>
#include <set>
#include <Rocket/Core.h>

class XMLNode;
class RocketWindowState
{
public:
	RocketWindowState(Rocket::Core::Context *context);
	virtual ~RocketWindowState();

	void initialize();
	void setState(const std::string &state);

protected:
	class Window
	{
	public:
		bool initializeFromXml(XMLNode *node);

		void open(Rocket::Core::Context *context);
		void close(Rocket::Core::Context *context);

		std::string getName() { return name_; }
	private:
		std::string name_;
		std::string file_;
		Rocket::Core::ElementDocument* document_;
	};
	class State
	{
	public:
		bool initializeFromXml(XMLNode *node);

		std::string getName() { return name_; }
		std::set<std::string> &getWindowNames() { return windowNames_; }
	private:
		std::string name_;
		std::set<std::string> windowNames_;
	};

	Rocket::Core::Context *context_;
	std::map<std::string, Window *> windows_;
	std::map<std::string, State *> states_;

	bool loadDefinitions();
private:

};

#endif
