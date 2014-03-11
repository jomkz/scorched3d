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

#if !defined(__INCLUDE_TemplateRendererh_INCLUDE__)
#define __INCLUDE_TemplateRendererh_INCLUDE__

#include <template/TemplateProvider.h>

class TemplateRenderer
{
public:
	TemplateRenderer();
	virtual ~TemplateRenderer();

	bool renderTemplate(TemplateData &data, TemplateProvider *baseContext,
		const std::string &templateName, std::string &result);
	bool renderTemplateToFile(TemplateData &data, TemplateProvider *baseContext,
		const std::string &templateName, const std::string &outFile);

private:
	void loadFile(const std::string &templateName, std::string &result);
	bool renderSubTemplate(TemplateData &data, TemplateProvider *baseContext, std::string templateFile, std::string &result);
	bool expectCharacter(std::string &templateFile, std::string::size_type position, char character);
	bool expectIdCharacter(std::string &templateFile, std::string::size_type position);
	bool expectNotCharacter(std::string &templateFile, std::string::size_type position, char character);
	bool expectStringEndingIn(std::string &templateFile, std::string::size_type position, char end, std::string &str);
	bool expectString(std::string &templateFile, std::string::size_type position, const std::string &str);
	std::string::size_type readVariableReference(TemplateData &data, std::string &templateFile, std::string::size_type position,
		TemplateProvider *currentContext, TemplateProvider *&variableResult);
	std::string::size_type readVariableName(std::string &templateFile, std::string::size_type position,
		std::string &variableName);
	std::string::size_type readIf(TemplateData &data, std::string &templateFile, std::string::size_type position,
		TemplateProvider *currentContext, bool &ifResult);
	std::string::size_type readFor(TemplateData &data, std::string &templateFile, std::string::size_type position,
		TemplateProvider *currentContext, std::string &varName, std::list<TemplateProvider *> &result);
	std::string::size_type readUrl(TemplateData &data, std::string &templateFile, std::string::size_type position,
		TemplateProvider *currentContext, std::string &urlName);
	std::string::size_type readInclude(TemplateData &data, std::string &templateFile, std::string::size_type position,
		TemplateProvider *currentContext, std::string &fileName);
	std::string::size_type findEnd(TemplateData &data, std::string &templateFile, std::string::size_type position,
		TemplateProvider *currentContext);
};

#endif // __INCLUDE_TemplateRendererh_INCLUDE__
