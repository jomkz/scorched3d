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

#include <template/TemplateRenderer.h>
#include <common/Defines.h>

TemplateRenderer::TemplateRenderer()
{
}

TemplateRenderer::~TemplateRenderer()
{
}

void TemplateRenderer::loadFile(const std::string &templateName, std::string &result)
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

bool TemplateRenderer::renderTemplateToFile(TemplateData &data, TemplateProvider *baseContext,
	const std::string &templateName, const std::string &outFile)
{
	std::string result;
	if (!renderTemplate(data, baseContext, templateName, result)) return false;
	FILE *out = fopen(outFile.c_str(), "w");
	if (!out) return false;
	fwrite(result.c_str(), result.size(), 1, out);
	fclose(out);
	return true;
}

bool TemplateRenderer::renderTemplate(TemplateData &data, TemplateProvider *baseContext,
	const std::string &templateName, std::string &result)
{
	std::string templateFile;
	loadFile(templateName, templateFile);
	std::list<std::string> functionStack;
	return renderSubTemplate(data, baseContext, templateFile, result, functionStack);
}

bool TemplateRenderer::renderSubTemplate(TemplateData &data, TemplateProvider *baseContext,
	std::string templateFile, std::string &result, std::list<std::string> &functionStack)
{
	std::string::size_type position = 0;
	while (position < templateFile.length()) 
	{
		char c = templateFile[position];
		if (c == '$')
		{
			if (expectCharacter(templateFile, position, '$'))
			{
				std::string variableName;
				TemplateProvider *variableResult = 0;
				std::string::size_type newPosition = readVariableReference(data, templateFile, position, baseContext, variableResult);
				if (newPosition != 0)
				{
					position = newPosition;
					if (variableResult) {
						std::string tmpResult;
						variableResult->getStringProperty(data, tmpResult);
						renderSubTemplate(data, baseContext, tmpResult, result, functionStack);
					}
					continue;
				}
			}
		}
		else if (c == '#')
		{
			if (expectString(templateFile, position, "#url"))
			{
				std::string urlName;
				std::string::size_type newPosition = readUrl(data, templateFile, position, baseContext, urlName);
				if (newPosition != 0)
				{
					position = newPosition;
					result.append(S3D::formatStringBuffer("<a href='%s'>%s</a>",
						urlName.c_str(), urlName.c_str()));
					continue;
				}
			}
			else if (expectString(templateFile, position, "#include"))
			{
				std::string fileName;
				std::string::size_type newPosition = readInclude(data, templateFile, position, baseContext, fileName);
				if (newPosition != 0)
				{
					position = newPosition;
					std::string tmpResult;
					loadFile(fileName, tmpResult);
					renderSubTemplate(data, baseContext, tmpResult, result, functionStack);
					continue;
				}
			}
			else if (expectString(templateFile, position, "#call"))
			{
				std::string functionName;
				std::string::size_type newPosition = readCallFunction(data, templateFile, position, baseContext, result, functionStack);
				if (newPosition != 0)
				{
					position = newPosition;
					continue;
				}
			}
			else if (expectString(templateFile, position, "#if"))
			{
				bool ifResult = false;
				std::string::size_type newPosition = readIf(data, templateFile, position, baseContext, ifResult);
				if (newPosition != 0)
				{
					position = newPosition;
					newPosition = findEnd(data, templateFile, position, baseContext);
					if (newPosition == 0) S3D::dialogExit("FileTemplate",
						S3D::formatStringBuffer("Failed to find #end for #if, position %u", position));

					if (ifResult)
					{
						std::string newTemplateFile(templateFile, position, newPosition - position);
						if (!renderSubTemplate(data, baseContext, newTemplateFile, result, functionStack))
						{
							return false;
						}
					}

					position = newPosition + 4; // #end
					continue;
				}
			}
			else if (expectString(templateFile, position, "#function"))
			{
				Function functionDefinition;
				std::string::size_type newPosition = readFunction(data, templateFile, position, baseContext, functionDefinition);
				if (newPosition != 0)
				{
					position = newPosition;
					newPosition = findEnd(data, templateFile, position, baseContext);
					if (newPosition == 0) S3D::dialogExit("FileTemplate",
						S3D::formatStringBuffer("Failed to find #end for #function, position %u", position));

					std::string functionContents(templateFile, position, newPosition - position);
					functionDefinition.content = functionContents;
					functions_[functionDefinition.name] = functionDefinition;

					position = newPosition + 4; // #end
					continue;
				}
			}
			else if (expectString(templateFile, position, "#for"))
			{
				std::string varName;
				std::list<TemplateProvider *> forList;
				std::string::size_type newPosition = readFor(data, templateFile, position, baseContext, varName, forList);
				if (newPosition != 0)
				{
					position = newPosition;
					newPosition = findEnd(data, templateFile, position, baseContext);
					if (newPosition == 0) S3D::dialogExit("FileTemplate",
						S3D::formatStringBuffer("Failed to find #end for #for, position %u", position));

					bool odd = false;
					std::string newTemplateFile(templateFile, position, newPosition - position);
					std::list<TemplateProvider *>::iterator itor = forList.begin(), end = forList.end();
					for (; itor != end; ++itor)
					{
						TemplateProviderLocal local(baseContext);
						TemplateProviderString odd_row(odd?"true":"false");
						local.addLocalVariable("ODD_ROW", &odd_row);
						local.addLocalVariable(varName, *itor);
						if (!renderSubTemplate(data, &local, newTemplateFile, result, functionStack))
						{
							return false;
						}
						odd = !odd;
					}

					position = newPosition + 4; // #end
					continue;
				}
			}
		}

		result.push_back(c);
		position++;
	}
	return true;
}

std::string::size_type TemplateRenderer::readVariableName(std::string &templateFile, std::string::size_type position,
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

std::string::size_type TemplateRenderer::readVariableReference(TemplateData &data, std::string &templateFile, std::string::size_type position,
	TemplateProvider *currentContext, TemplateProvider *&variableResult)
{
	variableResult = 0;
	std::string variableName;
	std::string::size_type result = readVariableName(templateFile, position, variableName);
	if (result == 0) return result;

	if (variableName.empty()) return 0;
	for (char *token = strtok((char *)variableName.c_str(), ".");
		token != 0;
		token = strtok(0, "."))
	{
		if (currentContext) 
		{
			currentContext = currentContext->getChild(data, token);
		}
	}
	variableResult = currentContext;

	return result;
}

std::string::size_type TemplateRenderer::readUrl(TemplateData &data, std::string &templateFile, std::string::size_type position,
	TemplateProvider *currentContext, std::string &urlName)
{
	if (!expectString(templateFile, position, "#url(")) return 0;
	position += 5;
	int variableStartPosition = position;
	while (expectIdCharacter(templateFile, position))
	{
		position++;
	}
	if (!expectCharacter(templateFile, position, ')')) return 0;

	urlName = templateFile.substr(variableStartPosition, position - variableStartPosition);
	if (urlName.empty()) return 0;

	position++;
	return position;
}

std::string::size_type TemplateRenderer::readInclude(TemplateData &data, std::string &templateFile, std::string::size_type position,
	TemplateProvider *currentContext, std::string &fileName)
{
	if (!expectString(templateFile, position, "#include(")) return 0;
	position += 9;
	if (!expectStringEndingIn(templateFile, position, ')', fileName)) return 0;
	position += fileName.length();
	if (!expectCharacter(templateFile, position, ')')) return 0;
	position++;
	return position;
}

std::string::size_type TemplateRenderer::readFunction(TemplateData &data, std::string &templateFile, std::string::size_type position,
	TemplateProvider *currentContext, Function &functionResult)
{
	if (!expectString(templateFile, position, "#function(")) return 0;
	position += 10;
	int variableStartPosition = position;
	while (expectIdCharacter(templateFile, position))
	{
		position++;
	}
	if (!expectCharacter(templateFile, position, '(')) return 0;
	position++;

	functionResult.name = templateFile.substr(variableStartPosition, position - variableStartPosition);
	if (functionResult.name.empty()) return 0;
	
	int namePos = 0;
	std::string variableName;
	while ((namePos = readVariableName(templateFile, position, variableName)) != 0)
	{
		position = namePos;
		functionResult.parameters.push_back(variableName);
		if (variableName.empty()) return 0;
		if (expectCharacter(templateFile, position, ',')) position++;
	}
	if (!expectCharacter(templateFile, position, ')')) return 0;
	position++;
	if (!expectCharacter(templateFile, position, ')')) return 0;
	position++;

	return position;
}

std::string::size_type TemplateRenderer::readCallFunction(TemplateData &data, std::string &templateFile, std::string::size_type position,
	TemplateProvider *currentContext, std::string &result, std::list<std::string> &functionStack)
{
	if (!expectString(templateFile, position, "#call(")) return 0;
	position += 6;
	int variableStartPosition = position;
	while (expectIdCharacter(templateFile, position))
	{
		position++;
	}
	if (!expectCharacter(templateFile, position, '(')) return 0;
	position++;

	std::string functionName = templateFile.substr(variableStartPosition, position - variableStartPosition);
	if (functionName.empty()) return 0;

	std::map<std::string, Function>::iterator itor = functions_.find(functionName);
	if (itor == functions_.end()) return 0;
	Function &function = itor->second;

	int namePos = 0;
	TemplateProviderLocal local(currentContext);
	TemplateProvider *variableResult;
	int i = 0;
	while ((namePos = readVariableReference(data, templateFile, position, currentContext, variableResult)) != 0)
	{
		position = namePos;
		if (!variableResult) return 0;

		local.addLocalVariable(function.parameters[i], variableResult);
		if (expectCharacter(templateFile, position, ',')) position++;
		i++;
	}
	if (!expectCharacter(templateFile, position, ')')) return 0;
	position++;
	if (!expectCharacter(templateFile, position, ')')) return 0;
	position++;

	if (functionStack.size() < 10)
	{
		functionStack.push_back("Hmm");
		if (!renderSubTemplate(data, &local, function.content, result, functionStack)) return 0;
		functionStack.pop_back();
	}

	return position;
}

std::string::size_type TemplateRenderer::readIf(TemplateData &data, std::string &templateFile, std::string::size_type position,
	TemplateProvider *currentContext, bool &ifResult)
{
	if (!expectString(templateFile, position, "#if(")) return 0;
	position += 4;
	TemplateProvider *variableResult = 0;
	int newPosition = readVariableReference(data, templateFile, position, currentContext, variableResult);
	if (newPosition == 0) return 0;
	position = newPosition;
	if (expectCharacter(templateFile, position, ')'))
	{
		ifResult = (variableResult != 0);
		position++;
		return position;
	}
	std::string id = "";
	if (expectString(templateFile, position, "=~")) id = "=~";
	if (expectString(templateFile, position, "!~")) id = "!~";
	if (expectString(templateFile, position, "==")) id = "==";
	if (expectString(templateFile, position, "!=")) id = "!=";
	if (!id.empty())
	{
		position += 2;
		std::string match;
		if (!expectStringEndingIn(templateFile, position, ')', match)) return 0;
		position += match.length();
		if (!variableResult)
		{
			if (id == "!~") ifResult = true;
			else if (id == "!=") ifResult = true;
			else ifResult = false;
		}
		else
		{
			std::string stringProperty;
			variableResult->getStringProperty(data, stringProperty);
			if (id == "=~")	ifResult = (stringProperty.find(match) != std::string::npos);
			else if (id == "!~") ifResult = (stringProperty.find(match) == std::string::npos);
			else if (id == "==") ifResult = (match == stringProperty);
			else if (id == "!=") ifResult = (match != stringProperty);
		}
	}
	if (expectCharacter(templateFile, position, ')'))
	{
		position++;
		return position;
	}
	return 0;
}

std::string::size_type TemplateRenderer::readFor(TemplateData &data, std::string &templateFile, std::string::size_type position,
	TemplateProvider *currentContext, std::string &varName, std::list<TemplateProvider *> &result)
{
	if (!expectString(templateFile, position, "#for(")) return 0;
	position += 5;
	int newPosition = readVariableName(templateFile, position, varName);
	if (newPosition == 0) return 0;
	position = newPosition;
	if (!expectString(templateFile, position, " in ")) return 0;
	position += 4;
	TemplateProvider *variableResult = 0;
	newPosition = readVariableReference(data, templateFile, position, currentContext, variableResult);
	if (newPosition == 0) return 0;
	position = newPosition;
	if (variableResult)
	{
		variableResult->getListProperty(data, result);
	}
	if (!expectCharacter(templateFile, position, ')')) return 0;
	position++;
	return position;
}

std::string::size_type TemplateRenderer::findEnd(TemplateData &data, std::string &templateFile, std::string::size_type position,
	TemplateProvider *currentContext)
{
	int depth = 0;
	while (position < templateFile.length())
	{
		char c = templateFile[position];
		if (c == '#')
		{
			if (expectString(templateFile, position, "#end"))
			{
				if (depth == 0)	return position;
				depth--;
			}
			else if (expectString(templateFile, position, "#if"))
			{
				bool ifResult;
				std::string::size_type newPosition = readIf(data, templateFile, position, currentContext, ifResult);
				if (newPosition != 0)
				{
					depth++;
				}
			}
			else if (expectString(templateFile, position, "#function"))
			{
				Function functionResult;
				std::string::size_type newPosition = readFunction(data, templateFile, position, currentContext, functionResult);
				if (newPosition != 0)
				{
					depth++;
				}
			}
			else if (expectString(templateFile, position, "#for"))
			{
				std::list<TemplateProvider *> result;
				std::string varName;
				std::string::size_type newPosition = readFor(data, templateFile, position, currentContext, varName, result);
				if (newPosition != 0)
				{
					depth++;
				}
			}
		}
		position++;
	}
	return 0;
}

bool TemplateRenderer::expectCharacter(std::string &templateFile, std::string::size_type position, char character)
{
	if (position >= templateFile.length()) return false;
	char c = templateFile[position];
	return (c == character);
}

bool TemplateRenderer::expectNotCharacter(std::string &templateFile, std::string::size_type position, char character)
{
	if (position >= templateFile.length()) return false;
	char c = templateFile[position];
	return (c != character);
}

bool TemplateRenderer::expectString(std::string &templateFile, std::string::size_type position, const std::string &str)
{
	for (std::string::size_type i = 0; i < str.length(); i++)
	{
		char c = str[i];
		if (!expectCharacter(templateFile, position, c)) return false;
		position++;
	}
	return true;
}

bool TemplateRenderer::expectStringEndingIn(std::string &templateFile, std::string::size_type position, char end, std::string &str)
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

bool TemplateRenderer::expectIdCharacter(std::string &templateFile, std::string::size_type position)
{
	if (position >= templateFile.length()) return false;
	char c = templateFile[position];
	return c == '.' || c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
