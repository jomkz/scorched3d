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

#if !defined(__INCLUDE_TemplateParserh_INCLUDE__)
#define __INCLUDE_TemplateParserh_INCLUDE__

#include <string>

class TemplateParser
{
public:
	static bool expectCharacter(std::string &templateFile, std::string::size_type position, char character);
	static bool expectIdCharacter(std::string &templateFile, std::string::size_type position);
	static bool expectNotCharacter(std::string &templateFile, std::string::size_type position, char character);
	static bool expectStringEndingIn(std::string &templateFile, std::string::size_type position, char end, std::string &str);
	static bool expectString(std::string &templateFile, std::string::size_type position, const std::string &str);
	static std::string::size_type readQuotedString(std::string &templateFile, std::string::size_type position, 
		std::string &str);
	static std::string::size_type readVariableName(std::string &templateFile, std::string::size_type position,
		std::string &variableName);
	static void loadFile(const std::string &templateName, std::string &result);
};
#endif // __INCLUDE_TemplateParserh_INCLUDE__
