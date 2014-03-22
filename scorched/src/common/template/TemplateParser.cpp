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

#include <template/TemplateParser.h>
#include <common/Defines.h>

bool TemplateParser::expectCharacter(std::string &templateFile, std::string::size_type position, char character)
{
	if (position >= templateFile.length()) return false;
	char c = templateFile[position];
	return (c == character);
}

bool TemplateParser::expectNotCharacter(std::string &templateFile, std::string::size_type position, char character)
{
	if (position >= templateFile.length()) return false;
	char c = templateFile[position];
	return (c != character);
}

bool TemplateParser::expectString(std::string &templateFile, std::string::size_type position, const std::string &str)
{
	for (std::string::size_type i = 0; i < str.length(); i++)
	{
		char c = str[i];
		if (!expectCharacter(templateFile, position, c)) return false;
		position++;
	}
	return true;
}

bool TemplateParser::expectStringEndingIn(std::string &templateFile, std::string::size_type position, char end, std::string &str)
{
	str = "";
	while (expectNotCharacter(templateFile, position, end))
	{
		char c = templateFile[position];
		str.push_back(c);
		position++;
	}
	if (str.empty()) return false;
	return true;
}

bool TemplateParser::expectIdCharacter(std::string &templateFile, std::string::size_type position)
{
	if (position >= templateFile.length()) return false;
	char c = templateFile[position];
	return c == '.' || c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

std::string::size_type TemplateParser::readVariableName(std::string &templateFile, std::string::size_type position,
	std::string &variableName)
{
	if (!expectCharacter(templateFile, position, '$')) return 0;
	position++;
	if (!expectCharacter(templateFile, position, '{')) return 0;
	position++;
	int variableStartPosition = position;
	while (expectIdCharacter(templateFile, position))
	{
		position++;
	}
	if (!expectCharacter(templateFile, position, '}')) return 0;

	variableName = templateFile.substr(variableStartPosition, position - variableStartPosition);
	if (variableName.empty()) return 0;

	position++;
	return position;
}

std::string::size_type TemplateParser::readQuotedString(std::string &templateFile, std::string::size_type position,
	std::string &str)
{
	if (!expectCharacter(templateFile, position, '\"')) return 0;
	position++;
	if (!expectStringEndingIn(templateFile, position, '\"', str)) return 0;
	position += str.length();
	if (!expectCharacter(templateFile, position, '\"')) return 0;
	position++;

	if (str.empty()) return 0;
	return position;
}

void TemplateParser::loadFile(const std::string &templateName, std::string &result)
{
	// Perhaps cache this
	std::string fileName = S3D::getDataFile(
		S3D::formatStringBuffer("data/templates/%s", templateName.c_str()));
	FILE *in = fopen(fileName.c_str(), "r");
	if (!in) S3D::dialogExit("FileTemplate",
		S3D::formatStringBuffer("Failed to find file template %s", fileName.c_str()));

	char buffer[1024];
	while (fgets(buffer, 1024, in))
	{
		result += buffer;
	}
	fclose(in);
}
