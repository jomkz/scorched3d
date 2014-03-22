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

#include <template/Template.h>
#include <template/TemplateParser.h>
#include <common/Defines.h>

std::string::size_type Expression::readExpression(std::string &templateFile, std::string::size_type position, Expression *&result)
{
	result = 0;
	{
		std::string varName;
		std::string::size_type newPosition = TemplateParser::readVariableName(templateFile, position, varName);
		if (newPosition != 0)
		{
			result = new ExpressionVariableReference(varName);
			return newPosition;
		}
	}
	{
		std::string str;
		std::string::size_type newPosition = TemplateParser::readQuotedString(templateFile, position, str);
		if (newPosition != 0)
		{
			result = new ExpressionString(str);
			return newPosition;
		}
	}

	return 0;
}

TemplateProvider *ExpressionVariableReference::runExpression(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
{
	TemplateProvider *variableResult = getVariableValue(renderContext.data, currentContext, name_);
	return variableResult;
}

TemplateProvider *ExpressionVariableReference::getVariableValue(TemplateData &data, TemplateProvider *currentContext, std::string &variableName)
{
	std::list<std::string> tokResult;
	S3D::strtok(variableName, ".", tokResult);
	std::list<std::string>::iterator tokItor = tokResult.begin(),
		tokEnd = tokResult.end();
	for (; tokItor != tokEnd; ++tokItor)
	{
		if (currentContext)
		{
			currentContext = currentContext->getChild(data, *tokItor);
		}
	}
	return currentContext;
}

TemplateCompileContext::TemplateCompileContext()
{
}

TemplateCompileContext::~TemplateCompileContext()
{
	std::map<std::string, TemplateFunctionDeclaration *>::iterator itor =
		functions_.begin(), end = functions_.end();
	for (; itor != end; ++itor)
	{
		delete itor->second;
	}
	functions_.clear();
}

TemplateBlock::TemplateBlock()
{
}

TemplateBlock::~TemplateBlock()
{
	std::list<TemplateAction *>::iterator itor = actions_.begin(),
		end = actions_.end();
	for (; itor != end; ++itor)
	{
		delete *itor;
	}
	actions_.clear();
}

void TemplateBlock::renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
{
	TemplateProviderLocal local(currentContext);
	TemplateProviderString stackDepth(S3D::formatStringBuffer("%u", renderContext.stackDepth));
	local.addLocalVariable("STACK_DEPTH", &stackDepth);
	std::list<TemplateAction *>::iterator itor = actions_.begin(),
		end = actions_.end();
	for (; itor != end; ++itor)
	{
		TemplateAction *action = *itor;
		action->renderAction(renderContext, &local);
	}
}

void TemplateBlock::checkTextBlock(std::string &currentTextBlock)
{
	if (currentTextBlock.empty()) return;
	actions_.push_back(new TemplateTextBlock(currentTextBlock));
	currentTextBlock = "";
}

bool TemplateBlock::compile(TemplateCompileContext &compileContext, std::string &templateDefinition)
{
	std::string currentTextBlock;
	std::string::size_type position = 0;
	while (position < templateDefinition.length())
	{
		char c = templateDefinition[position];
		if (c == '$')
		{
			std::string variableName;
			std::string::size_type newPosition = TemplateParser::readVariableName(templateDefinition, position, variableName);
			if (newPosition != 0)
			{
				checkTextBlock(currentTextBlock);

				position = newPosition;
				TemplateVariableReference *variableReference = new TemplateVariableReference(variableName);
				actions_.push_back(variableReference);
				continue;
			}
		}
		else if (c == '#')
		{
			if (TemplateParser::expectString(templateDefinition, position, "#url"))
			{
				std::string urlName, urlUri;
				std::string::size_type newPosition = readUrl(templateDefinition, position, urlName, urlUri);
				if (newPosition != 0)
				{
					position = newPosition;
					currentTextBlock.append(S3D::formatStringBuffer("<a href='%s'>%s</a>",
						urlUri.c_str(), urlName.c_str()));
					continue;
				}
				else S3D::dialogExit("TemplateParser", "Unable to parse #url");
			}
			else if (TemplateParser::expectString(templateDefinition, position, "#eval"))
			{
				TemplateVariableReferenceEval *eval = 0;
				std::string::size_type newPosition = readEval(templateDefinition, position,compileContext, eval);
				if (newPosition != 0)
				{
					checkTextBlock(currentTextBlock);
					if (eval) actions_.push_back(eval);

					position = newPosition;
					continue;
				}
				else S3D::dialogExit("TemplateParser", "Unable to parse #eval");
			}
			else if(TemplateParser::expectString(templateDefinition, position, "#include"))
			{
				std::string fileName;
				std::string::size_type newPosition = readInclude(templateDefinition, position, fileName);
				if (newPosition != 0)
				{
					checkTextBlock(currentTextBlock);

					position = newPosition;
					std::string tmpResult;
					TemplateParser::loadFile(fileName, tmpResult);
					compile(compileContext, tmpResult);
					continue;
				}
				else S3D::dialogExit("TemplateParser", "Unable to parse #include");
			}
			else if (TemplateParser::expectString(templateDefinition, position, "#call"))
			{
				TemplateFunctionReference *functionReference = 0;
				std::string::size_type newPosition = readCallFunction(templateDefinition, position, compileContext, functionReference);
				if (newPosition != 0)
				{
					checkTextBlock(currentTextBlock);
					if (functionReference) actions_.push_back(functionReference);

					position = newPosition;
					continue;
				}
				else S3D::dialogExit("TemplateParser", "Unable to parse #call");
			}
			else if (TemplateParser::expectString(templateDefinition, position, "#function"))
			{
				TemplateFunctionDeclaration *functionDefinition = 0;
				std::string::size_type newPosition = readFunction(templateDefinition, position, compileContext, functionDefinition);
				if (newPosition != 0)
				{
					checkTextBlock(currentTextBlock);
					if (functionDefinition) compileContext.functions_[functionDefinition->getName()] = functionDefinition;

					position = newPosition;
					continue;
				}
				else S3D::dialogExit("TemplateParser", "Unable to parse #function");
			}
			else if (TemplateParser::expectString(templateDefinition, position, "#if"))
			{
				TemplateIfDeclaration *ifDefinition = 0;
				std::string::size_type newPosition = readIf(templateDefinition, position, compileContext, ifDefinition);
				if (newPosition != 0)
				{
					checkTextBlock(currentTextBlock);
					if (ifDefinition) actions_.push_back(ifDefinition);

					position = newPosition;
					continue;
				}
				else S3D::dialogExit("TemplateParser", "Unable to parse #if");
			}
			else if (TemplateParser::expectString(templateDefinition, position, "#for"))
			{
				TemplateForDeclaration *forDefinition = 0;
				std::string::size_type newPosition = readFor(templateDefinition, position, compileContext, forDefinition);
				if (newPosition != 0)
				{
					checkTextBlock(currentTextBlock);
					if (forDefinition) actions_.push_back(forDefinition);

					position = newPosition;
					continue;
				}
				else S3D::dialogExit("TemplateParser", "Unable to parse #for");
			}
		}

		currentTextBlock.push_back(c);
		position++;
	}
	checkTextBlock(currentTextBlock);
	return true;
}

std::string::size_type TemplateBlock::readUrl(std::string &templateFile, std::string::size_type position, 
	std::string &urlName, std::string &urlUri)
{
	if (!TemplateParser::expectString(templateFile, position, "#url(")) return 0;
	position += 5;
	if (!TemplateParser::expectStringEndingIn(templateFile, position, ':', urlName)) return 0;
	position += urlName.size() + 1;
	if (!TemplateParser::expectStringEndingIn(templateFile, position, ')', urlUri)) return 0;
	position += urlUri.size();
	if (!TemplateParser::expectCharacter(templateFile, position, ')')) return 0;
	position++;

	if (urlName.empty()) return 0;
	if (urlUri.empty()) return 0;
	return position;
}

std::string::size_type TemplateBlock::readInclude(std::string &templateFile, std::string::size_type position, std::string &fileName)
{
	if (!TemplateParser::expectString(templateFile, position, "#include(")) return 0;
	position += 9;
	if (!TemplateParser::expectStringEndingIn(templateFile, position, ')', fileName)) return 0;
	position += fileName.length();
	if (!TemplateParser::expectCharacter(templateFile, position, ')')) return 0;
	position++;
	return position;
}

std::string::size_type TemplateBlock::readEval(std::string &templateFile, std::string::size_type position,
	TemplateCompileContext &compileContext, TemplateVariableReferenceEval *&eval)
{
	if (!TemplateParser::expectString(templateFile, position, "#eval(")) return 0;
	position += 6;
	std::string variableName = "";
	position = TemplateParser::readVariableName(templateFile, position, variableName);
	if (position == 0) return 0;
	if (!TemplateParser::expectCharacter(templateFile, position, ')')) return 0;
	position++;
	eval = new TemplateVariableReferenceEval(variableName);
	return position;
}

std::string::size_type TemplateBlock::readCallFunction(std::string &templateFile, std::string::size_type position,
	TemplateCompileContext &compileContext, TemplateFunctionReference *&functionReference)
{
	if (!TemplateParser::expectString(templateFile, position, "#call(")) return 0;
	position += 6;
	int variableStartPosition = position;
	while (TemplateParser::expectIdCharacter(templateFile, position))
	{
		position++;
	}

	std::string functionName = templateFile.substr(variableStartPosition, position - variableStartPosition);
	if (functionName.empty()) return 0;

	if (!TemplateParser::expectCharacter(templateFile, position, '(')) return 0;
	position++;

	int namePos = 0;
	std::list<Expression *> variables;
	Expression *variableResult = 0;
	while ((namePos = Expression::readExpression(templateFile, position, variableResult)) != 0)
	{
		position = namePos;
		if (variableResult == 0) return 0;

		variables.push_back(variableResult);
		if (TemplateParser::expectCharacter(templateFile, position, ',')) position++;
	}
	if (!TemplateParser::expectCharacter(templateFile, position, ')')) return 0;
	position++;
	if (!TemplateParser::expectCharacter(templateFile, position, ')')) return 0;
	position++;

	functionReference = new TemplateFunctionReference(&compileContext, functionName, variables);

	return position;
}

std::string::size_type TemplateBlock::readFunction(std::string &templateFile, std::string::size_type position,
	TemplateCompileContext &compileContext, TemplateFunctionDeclaration *&functionDeclaration)
{
	if (!TemplateParser::expectString(templateFile, position, "#function(")) return 0;
	position += 10;
	int variableStartPosition = position;
	while (TemplateParser::expectIdCharacter(templateFile, position))
	{
		position++;
	}

	std::string functionName = templateFile.substr(variableStartPosition, position - variableStartPosition);
	if (functionName.empty()) return 0;

	if (!TemplateParser::expectCharacter(templateFile, position, '(')) return 0;
	position++;

	int namePos = 0;
	std::string variableName;
	std::list<std::string> parameters;
	while ((namePos = TemplateParser::readVariableName(templateFile, position, variableName)) != 0)
	{
		position = namePos;
		parameters.push_back(variableName);
		if (variableName.empty()) return 0;
		if (TemplateParser::expectCharacter(templateFile, position, ',')) position++;
	}
	if (!TemplateParser::expectCharacter(templateFile, position, ')')) return 0;
	position++;
	if (!TemplateParser::expectCharacter(templateFile, position, ')')) return 0;
	position++;

	int originalPosition = position;
	position = findEnd(templateFile, position, compileContext);
	if (position == 0) S3D::dialogExit("FileTemplate",
		S3D::formatStringBuffer("Failed to find #end for #function, position %u", position));

	std::string functionContents(templateFile, originalPosition, position - originalPosition);
	functionDeclaration = new TemplateFunctionDeclaration(functionName, parameters);
	if (!functionDeclaration->getBlock().compile(compileContext, functionContents)) return 0;
	
	position += 4; // #end
	return position;
}

std::string::size_type TemplateBlock::readIf(std::string &templateFile, std::string::size_type position,
	TemplateCompileContext &compileContext, TemplateIfDeclaration *&ifDeclaration)
{
	if (!TemplateParser::expectString(templateFile, position, "#if(")) return 0;
	position += 4;
	std::string variableName;
	int newPosition = TemplateParser::readVariableName(templateFile, position, variableName);
	if (newPosition == 0) return 0;
	position = newPosition;
	std::string op = "", match = "";
	if (TemplateParser::expectString(templateFile, position, "=~")) op = "=~";
	if (TemplateParser::expectString(templateFile, position, "!~")) op = "!~";
	if (TemplateParser::expectString(templateFile, position, "==")) op = "==";
	if (TemplateParser::expectString(templateFile, position, "!=")) op = "!=";
	if (TemplateParser::expectString(templateFile, position, ">")) op = ">";
	if (TemplateParser::expectString(templateFile, position, "<")) op = "<";
	if (TemplateParser::expectString(templateFile, position, "<=")) op = "<=";
	if (TemplateParser::expectString(templateFile, position, ">=")) op = ">=";
	if (!op.empty())
	{
		position += op.size();
		position = TemplateParser::readQuotedString(templateFile, position, match);
		if (position == 0) return 0;
	}
	if (!TemplateParser::expectCharacter(templateFile, position, ')'))
	{
		return 0;
	}
	position++;
	
	int ifPosition = position;
	position = findEnd(templateFile, position, compileContext);
	if (position == 0) S3D::dialogExit("FileTemplate",
		S3D::formatStringBuffer("Failed to find #end for #if, position %u", position));

	std::string newTemplateFile(templateFile, ifPosition, position - ifPosition);
	ifDeclaration = new TemplateIfDeclaration(variableName, op, match);
	if (!ifDeclaration->getBlock().compile(compileContext, newTemplateFile)) return 0;

	position += 4;
	return position;
}

std::string::size_type TemplateBlock::readFor(std::string &templateFile, std::string::size_type position,
	TemplateCompileContext &compileContext, TemplateForDeclaration *&forDeclaration)
{
	if (!TemplateParser::expectString(templateFile, position, "#for(")) return 0;
	position += 5;
	std::string varName;
	int newPosition = TemplateParser::readVariableName(templateFile, position, varName);
	if (newPosition == 0) return 0;
	position = newPosition;
	if (!TemplateParser::expectString(templateFile, position, " in ")) return 0;
	position += 4;
	std::string variableReference;
	newPosition = TemplateParser::readVariableName(templateFile, position, variableReference);
	if (newPosition == 0) return 0;
	position = newPosition;
	if (!TemplateParser::expectCharacter(templateFile, position, ')')) return 0;
	position++;

	int forPosition = position;
	position = findEnd(templateFile, position, compileContext);
	if (position == 0) S3D::dialogExit("FileTemplate",
		S3D::formatStringBuffer("Failed to find #end for #for, position %u", position));

	std::string newTemplateFile(templateFile, forPosition, position - forPosition);
	forDeclaration = new TemplateForDeclaration(varName, variableReference);
	if (!forDeclaration->getBlock().compile(compileContext, newTemplateFile)) return 0;

	position += 4;

	return position;
}

std::string::size_type TemplateBlock::findEnd(std::string &templateFile, std::string::size_type position,
	TemplateCompileContext &compileContext)
{
	int depth = 0;
	while (position < templateFile.length())
	{
		char c = templateFile[position];
		if (c == '#')
		{
			if (TemplateParser::expectString(templateFile, position, "#end"))
			{
				if (depth == 0)	return position;
				depth--;
			}
			else if (TemplateParser::expectString(templateFile, position, "#if"))
			{
				TemplateCompileContext newContext;
				TemplateIfDeclaration *ifDeclaration = 0;
				std::string::size_type newPosition = readIf(templateFile, position, newContext, ifDeclaration);
				if (newPosition != 0)
				{
					delete ifDeclaration;
					depth++;
				}
			}
			else if (TemplateParser::expectString(templateFile, position, "#function"))
			{
				TemplateCompileContext newContext;
				TemplateFunctionDeclaration *functionDeclaration = 0;
				std::string::size_type newPosition = readFunction(templateFile, position, newContext, functionDeclaration);
				if (newPosition != 0)
				{
					depth++;
				}
			}
			else if (TemplateParser::expectString(templateFile, position, "#for"))
			{
				TemplateCompileContext newContext;
				TemplateForDeclaration *forDeclaration = 0;
				std::string::size_type newPosition = readFor(templateFile, position, newContext, forDeclaration);
				if (newPosition != 0)
				{
					delete forDeclaration;
					depth++;
				}
			}
		}
		position++;
	}
	return 0;
}

void TemplateVariableReference::renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
{
	TemplateProvider *variableResult = ExpressionVariableReference::getVariableValue(renderContext.data, currentContext, variableName_);
	if (variableResult) 
	{
		std::string tmpResult;
		variableResult->getStringProperty(renderContext.data, tmpResult);
		renderContext.result.append(tmpResult);
	}
}

void TemplateVariableReferenceEval::renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
{
	TemplateProvider *variableResult = ExpressionVariableReference::getVariableValue(renderContext.data, currentContext, variableName_);
	if (variableResult)
	{
		std::string tmpResult;
		variableResult->getStringProperty(renderContext.data, tmpResult);
		TemplateBlock newBlock;
		TemplateCompileContext compileContext;
		if (newBlock.compile(compileContext, tmpResult))
		{
			newBlock.renderAction(renderContext, currentContext);
		}
	}
}

TemplateFunctionReference::~TemplateFunctionReference()
{
	std::list<Expression *>::iterator itor = functionParams_.begin(),
		end = functionParams_.end();
	for (; itor != end; ++itor)
	{
		delete *itor;
	}
	functionParams_.clear();
}

void TemplateFunctionReference::renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
{
	if (!declaration_)
	{
		std::map<std::string, TemplateFunctionDeclaration *>::iterator itor = compileContext_->functions_.find(functionName_);
		if (itor == compileContext_->functions_.end()) S3D::dialogExit("TemplateBlock",
			S3D::formatStringBuffer("Failed to find function %s", functionName_.c_str()));
		declaration_ = itor->second;
		if (declaration_->getParams().size() != functionParams_.size()) S3D::dialogExit("TemplateBlock",
			S3D::formatStringBuffer("Failed to find function %s with %u parameters", functionName_.c_str(), functionParams_.size()));
	}

	TemplateProviderLocal local(currentContext);
	std::list<Expression *>::iterator itor = functionParams_.begin(),
		end = functionParams_.end();
	std::list<std::string>::iterator itor2 = declaration_->getParams().begin(),
		end2 = declaration_->getParams().end();
	for (; itor != end && itor2 != end2; ++itor, ++itor2)
	{
		TemplateProvider *value = (*itor)->runExpression(renderContext, currentContext);
		local.addLocalVariable(*itor2, value);
	}

	renderContext.stackDepth++;
	declaration_->getBlock().renderAction(renderContext, &local);
	renderContext.stackDepth--;
}

void TemplateFunctionDeclaration::renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
{
	templateBlock_.renderAction(renderContext, currentContext);
}

void TemplateIfDeclaration::renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
{
	TemplateProvider *variableResult = ExpressionVariableReference::getVariableValue(renderContext.data, currentContext, variableName_);
	bool ifResult = false;
	if (op_.empty())
	{
		ifResult = (variableResult != 0);
	}
	else
	{
		if (!variableResult)
		{
			if (op_ == "!~") ifResult = true;
			else if (op_ == "!=") ifResult = true;
			else ifResult = false;
		}
		else
		{
			std::string stringProperty;
			variableResult->getStringProperty(renderContext.data, stringProperty);
			if (op_ == "=~")	ifResult = (stringProperty.find(match_) != std::string::npos);
			else if (op_ == "!~") ifResult = (stringProperty.find(match_) == std::string::npos);
			else if (op_ == "==") ifResult = (match_ == stringProperty);
			else if (op_ == "!=") ifResult = (match_ != stringProperty);
			else if (op_ == "<") ifResult = (atoi(stringProperty.c_str()) < atoi(match_.c_str()));
			else if (op_ == "<=") ifResult = (atoi(stringProperty.c_str()) <= atoi(match_.c_str()));
			else if (op_ == ">") ifResult = (atoi(stringProperty.c_str()) > atoi(match_.c_str()));
			else if (op_ == ">=") ifResult = (atoi(stringProperty.c_str()) >= atoi(match_.c_str()));
		}
	}
	if (ifResult) templateBlock_.renderAction(renderContext, currentContext);
}

void TemplateForDeclaration::renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
{
	TemplateProvider *variableResult = ExpressionVariableReference::getVariableValue(renderContext.data, currentContext, varReference_);
	if (!variableResult) return;

	std::list<TemplateProvider *> forList;
	variableResult->getListProperty(renderContext.data, forList);
	bool odd = false;
	std::list<TemplateProvider *>::iterator itor = forList.begin(), end = forList.end();
	for (; itor != end; ++itor)
	{
		TemplateProviderLocal local(currentContext);
		TemplateProviderString odd_row(odd ? "true" : "false");
		local.addLocalVariable("ODD_ROW", &odd_row);
		local.addLocalVariable(variableName_, *itor);
		templateBlock_.renderAction(renderContext, &local);
		odd = !odd;
	}
}

Template::Template(const std::string &fileName) : fileName_(fileName)
{
}

Template::~Template()
{
}

bool Template::compile(std::string &contents)
{
	return templateBlock_.compile(compileContext_, contents);
}

void Template::render(TemplateData &data, std::string &result, TemplateProvider *currentContext)
{
	TemplateRenderContext context(data, result);
	templateBlock_.renderAction(context, currentContext);
}