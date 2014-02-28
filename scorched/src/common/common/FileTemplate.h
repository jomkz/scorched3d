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

#if !defined(AFX_FILETEMPLATE_H__6265A8BD_0FB0_4CB0_BDDC_6C706B2A2A07__INCLUDED_)
#define AFX_FILETEMPLATE_H__6265A8BD_0FB0_4CB0_BDDC_6C706B2A2A07__INCLUDED_

#include <list>
#include <map>
#include <set>
#include <string>

class FileTemplateVariables
{
public:
	FileTemplateVariables(FileTemplateVariables *parent = 0);
	~FileTemplateVariables();

	void addVariableValue(const char *name, const char *value, bool htmlEscape = false);
	FileTemplateVariables *addLoopVariable(const char *name);

	bool hasVariableName(const char *name);

	const char *getVariableValue(const char *name);
	bool hasPermission(const char *permission);
	FileTemplateVariables *getLoopVariableValues(const char *name);

private:
	FileTemplateVariables *parent_;
	std::map<std::string, std::string> variableValues_;
	std::map<std::string, std::list<FileTemplateVariables *> > loopVariableValues_;
	std::set<std::string> permissions_;
};

class FileTemplate
{
public:
	static bool getTemplate(const std::string &templateName, 
		FileTemplateVariables &variables, std::string &result);
	static bool writeTemplateToFile(const std::string &templateName, 
		FileTemplateVariables &variables, std::string &fileName);
};
#endif // !defined(AFX_FILETEMPLATE_H__6265A8BD_0FB0_4CB0_BDDC_6C706B2A2A07__INCLUDED_)
