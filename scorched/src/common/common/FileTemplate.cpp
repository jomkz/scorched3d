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

#include <common/FileTemplate.h>
#include <common/Defines.h>

FileTemplateVariables::FileTemplateVariables(FileTemplateVariables *parent) :
	parent_(parent)
{
}
FileTemplateVariables::~FileTemplateVariables()
{
	std::map<std::string, std::list<FileTemplateVariables *> >::iterator 
		itor = loopVariableValues_.begin(),
		end = loopVariableValues_.end();
	for (;itor!=end;++itor)
	{
		std::list<FileTemplateVariables *>::iterator itor2 = itor->second.begin(),
			end2 = itor->second.end();
		for (;itor2!=end2;++itor2)
		{
			delete *itor2;
		}
	}
}

bool FileTemplateVariables::hasVariableName(const char *name)
{
	return loopVariableValues_.find(name) != loopVariableValues_.end() ||
		variableValues_.find(name) != variableValues_.end();
}

void FileTemplateVariables::addVariableValue(const char *name, const char *value)
{
	variableValues_[name] = value;
}

FileTemplateVariables *FileTemplateVariables::addLoopVariable(const char *name)
{
	FileTemplateVariables *newVariables = new FileTemplateVariables(this);
	std::map<std::string, std::list<FileTemplateVariables *> >::iterator itor = loopVariableValues_.find(name);
	if (itor == loopVariableValues_.end())
	{
		loopVariableValues_[name] = std::list<FileTemplateVariables *>();
		itor = loopVariableValues_.find(name);
	}
	itor->second.push_back(newVariables);
	return newVariables;
}

FileTemplateVariables *FileTemplateVariables::getLoopVariableValues(const char *name)
{
	std::map<std::string, std::list<FileTemplateVariables *> >::iterator itor = loopVariableValues_.find(name);
	if (itor == loopVariableValues_.end()) return 0;
	if (itor->second.empty()) return 0;
	FileTemplateVariables *result = itor->second.front();
	itor->second.pop_front();
	return result;
}

const char *FileTemplateVariables::getVariableValue(const char *name)
{
	std::map<std::string, std::string>::iterator itor = variableValues_.find(name);
	if (itor == variableValues_.end())
	{
		if (parent_) return parent_->getVariableValue(name);
		return 0;
	}
	return itor->second.c_str();
}

bool FileTemplateVariables::hasPermission(const char *permission)
{
	if (permissions_.find(permission) == permissions_.end())
	{
		if (parent_) return parent_->hasPermission(permission);
		return false;
	}
	return true;
}

bool processTemplate(FileTemplateVariables &variables, std::string &result);
static bool performPermissionSubst(FileTemplateVariables &variables, std::string &result)
{
	// Find start {{permission}}
	size_t start1 = result.find("{{");
	if (start1 == std::string::npos) return false;
	size_t end1 = result.find("}}", start1);
	if (end1 == std::string::npos) return false;
	std::string perm(result, start1 + 2, end1 - start1 - 2);
	result.replace(start1, end1 - start1 + 2, "");

	// Find end {{permission}}
	size_t start2 = result.find(
		S3D::formatStringBuffer("{{%s}}", perm.c_str()), start1);
	if (start2 == std::string::npos) return false;
	result.replace(start2, 4 + perm.size(), "");

	std::string currentText = result.substr(start1, start2 - start1);
	std::string newText = "";

	size_t colon = perm.find(":");
	if (colon == std::string::npos) return false;
	std::string permOperator = perm.substr(0, colon);
	std::string permName = perm.substr(colon+1);

	if (permOperator == "perm")
	{
		if (variables.hasPermission(permName.c_str()))
		{
			newText = currentText;
		}
	}
	else if (permOperator == "loop")
	{
		for (;;)
		{
			FileTemplateVariables *loopVariables = variables.getLoopVariableValues(permName.c_str());
			if (!loopVariables) break;

			std::string newTemplateCopy = std::string(currentText);
			processTemplate(*loopVariables, newTemplateCopy);
			newText.append(newTemplateCopy);

			delete loopVariables; 
		}
	}
	else if (permOperator == "exists")
	{
		if (variables.hasVariableName(permName.c_str()))
		{
			newText = currentText;
		}
	}
	else
	{
		return false;
	}
	result.replace(start1, start2 - start1, newText);
	return true;
}

static bool performVariableSubst(FileTemplateVariables &variables, std::string &result)
{
	// Find start [[variable]]
	size_t start1 = result.find("[[");
	if (start1 == std::string::npos) return false;
	size_t end1 = result.find("]]", start1);
	if (end1 == std::string::npos) return false;
	std::string variableName(result, start1 + 2, end1 - start1 - 2);

	const char *variableValue = variables.getVariableValue(variableName.c_str());
	if (!variableValue) variableValue = variableName.c_str();
	result.replace(start1, end1 - start1 + 2, variableValue);
	return true;
}

static bool processTemplate(FileTemplateVariables &variables, std::string &result)
{
	for (;;)
	{
		if (!performPermissionSubst(variables, result)) break;
	}
	for (;;)
	{
		if (!performVariableSubst(variables, result)) break;
	}
	return true;
}

bool FileTemplate::getTemplate(const std::string &templateName, 
	FileTemplateVariables &variables, std::string &result)
{
	// Perhaps cache this
	std::string fileName = S3D::getDataFile(
		S3D::formatStringBuffer("data/templates/%s", templateName.c_str()));
	FILE *in = fopen(fileName.c_str(), "r");
	if (!in) S3D::dialogExit("FileTemplate", 
		S3D::formatStringBuffer("Failed to find file template %s", templateName.c_str()));

	char buffer[1024], include[256];
	while (fgets(buffer, 1024, in))
	{
		// Check for an include line
		if (sscanf(buffer, "#include %s", include) == 1)
		{
			// Add the included file
			std::string tmp;
			if (!getTemplate(include, variables, tmp))
			{
				return false;
			}

			result += tmp;
		}
		else
		{
			result += buffer;
		}
	}
	fclose(in);

	if (!processTemplate(variables, result))
	{
		return false;
	}
	return true;
}

bool FileTemplate::writeTemplateToFile(const std::string &templateName, 
	FileTemplateVariables &variables, std::string &fileName)
{
	std::string result;
	if (!FileTemplate::getTemplate(templateName, variables, result)) return false;
	FILE *out = fopen(fileName.c_str(), "w");
	if (!out) return false;
	fwrite(result.c_str(), result.size(), 1, out);
	fclose(out);
	return true;
}