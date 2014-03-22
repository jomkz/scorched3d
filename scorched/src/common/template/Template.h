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

#if !defined(__INCLUDE_Templateh_INCLUDE__)
#define __INCLUDE_Templateh_INCLUDE__

#include <template/TemplateProvider.h>
#include <vector>

class TemplateFunctionDeclaration;
class TemplateCompileContext
{
public:
	TemplateCompileContext();
	~TemplateCompileContext();

	std::map<std::string, TemplateFunctionDeclaration *> functions_;
};

class TemplateRenderContext
{
public:
	TemplateRenderContext(TemplateData &d, std::string &r) : data(d), result(r), stackDepth(0) {}
	~TemplateRenderContext() {}

	TemplateData &data;
	std::string &result;
	int stackDepth;
};

class Expression
{
public:
	Expression() {}
	virtual ~Expression() {}

	static std::string::size_type readExpression(std::string &templateFile, std::string::size_type position, Expression *&result);

	virtual TemplateProvider *runExpression(TemplateRenderContext &renderContext, TemplateProvider *currentContext) = 0;
};

class ExpressionString : public Expression
{
public:
	ExpressionString(const std::string &value) : string_(value) {}
	virtual ~ExpressionString() {}
	virtual TemplateProvider *runExpression(TemplateRenderContext &renderContext, TemplateProvider *currentContext)
	{
		return &string_;
	}

private:
	TemplateProviderString string_;
};

class ExpressionVariableReference : public Expression
{
public:
	ExpressionVariableReference(const std::string &name) : name_(name) {}
	virtual ~ExpressionVariableReference() {}
	virtual TemplateProvider *runExpression(TemplateRenderContext &renderContext, TemplateProvider *currentContext);

	static TemplateProvider *getVariableValue(TemplateData &data, TemplateProvider *currentContext, std::string &variableName);
private:
	std::string name_;
};

class TemplateAction
{
public:
	TemplateAction() {}
	virtual ~TemplateAction() {}

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext) = 0;
};

class TemplateFunctionReference;
class TemplateIfDeclaration;
class TemplateForDeclaration;
class TemplateVariableReferenceEval;
class TemplateBlock : public TemplateAction
{
public:
	TemplateBlock();
	virtual ~TemplateBlock();

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext);
	bool compile(TemplateCompileContext &compileContext, std::string &templateDefinition);
private:
	std::list<TemplateAction *> actions_;

	void checkTextBlock(std::string &currentTextBlock);

	std::string::size_type readUrl(std::string &templateFile, std::string::size_type position, 
		std::string &urlName, std::string &urlUri);
	std::string::size_type readInclude(std::string &templateFile, std::string::size_type position, std::string &fileName);
	std::string::size_type readEval(std::string &templateFile, std::string::size_type position, 
		TemplateCompileContext &compileContext, TemplateVariableReferenceEval *&eval);
	std::string::size_type readCallFunction(std::string &templateFile, std::string::size_type position,
		TemplateCompileContext &compileContext, TemplateFunctionReference *&functionReference);
	std::string::size_type readFunction(std::string &templateFile, std::string::size_type position,
		TemplateCompileContext &compileContext, TemplateFunctionDeclaration *&functionDeclaration);
	std::string::size_type readIf(std::string &templateFile, std::string::size_type position,
		TemplateCompileContext &compileContext, TemplateIfDeclaration *&ifDeclaration);
	std::string::size_type readFor(std::string &templateFile, std::string::size_type position,
		TemplateCompileContext &compileContext, TemplateForDeclaration *&forDeclaration);
	std::string::size_type findEnd(std::string &templateFile, std::string::size_type position,
		TemplateCompileContext &compileContext);
};

class TemplateTextBlock : public TemplateAction
{
public:
	TemplateTextBlock(const std::string &text) : text_(text) {}
	virtual ~TemplateTextBlock() {}

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext) { renderContext.result.append(text_); }

private:
	std::string text_;
};

class TemplateVariableReference : public TemplateAction
{
public:
	TemplateVariableReference(const std::string &variableName) : variableName_(variableName) {}
	virtual ~TemplateVariableReference() {}

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext);
private:
	std::string variableName_;
};

class TemplateVariableReferenceEval : public TemplateAction
{
public:
	TemplateVariableReferenceEval(const std::string &variableName) : variableName_(variableName) {}
	virtual ~TemplateVariableReferenceEval() {}

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext);
private:
	std::string variableName_;
};

class TemplateFunctionReference : public TemplateAction
{
public:
	TemplateFunctionReference(TemplateCompileContext *compileContext,
		std::string functionName,
		std::list<Expression *> functionParams) :
		declaration_(0),
		compileContext_(compileContext), 
		functionName_(functionName),
		functionParams_(functionParams) {}
	virtual ~TemplateFunctionReference();

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext);
private:
	TemplateCompileContext *compileContext_;
	TemplateFunctionDeclaration *declaration_;
	std::string functionName_;
	std::list<Expression *> functionParams_;
};

class TemplateFunctionDeclaration : public TemplateAction
{
public:
	TemplateFunctionDeclaration(std::string &name, std::list<std::string> &params) : name_(name), params_(params) {}
	virtual ~TemplateFunctionDeclaration() {}

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext);

	std::list<std::string> &getParams() { return params_; }
	std::string &getName() { return name_; }
	TemplateBlock &getBlock() { return templateBlock_; }
private:
	std::list<std::string> params_;
	std::string name_;
	TemplateBlock templateBlock_;
};

class TemplateIfDeclaration : public TemplateAction
{
public:
	TemplateIfDeclaration(std::string &variableName, std::string &op, std::string &match) : 
		variableName_(variableName), op_(op), match_(match) {}
	virtual ~TemplateIfDeclaration() {}

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext);

	TemplateBlock &getBlock() { return templateBlock_; }
private:
	TemplateBlock templateBlock_;
	std::string variableName_, op_, match_;
};

class TemplateForDeclaration : public TemplateAction
{
public:
	TemplateForDeclaration(std::string &variableName, std::string &varReference) : 
		variableName_(variableName), varReference_(varReference) {}
	virtual ~TemplateForDeclaration() {}

	virtual void renderAction(TemplateRenderContext &renderContext, TemplateProvider *currentContext);

	TemplateBlock &getBlock() { return templateBlock_; }
private:
	TemplateBlock templateBlock_;
	std::string variableName_, varReference_;
};

class Template
{
public:
	Template(const std::string &fileName);
	virtual ~Template();

	void render(TemplateData &data, std::string &result, TemplateProvider *currentContext);
	bool compile(std::string &contents);
private:
	TemplateCompileContext compileContext_;
	TemplateBlock templateBlock_;
	std::string fileName_;
};

#endif // __INCLUDE_Templateh_INCLUDE__
