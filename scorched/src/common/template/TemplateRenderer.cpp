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
#include <template/TemplateParser.h>
#include <template/Template.h>
#include <common/Defines.h>

TemplateRenderer::TemplateRenderer()
{
}

TemplateRenderer::~TemplateRenderer()
{
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
	Template *temp = 0;
	std::map<std::string, Template *>::iterator itor = templates_.find(templateName);
	if (itor == templates_.end())
	{
		temp = new Template(templateName);
		std::string templateFile;
		TemplateParser::loadFile(templateName, templateFile);
		if (!temp->compile(templateFile)) S3D::dialogExit("TemplateRenderer",
			"Failed to compile template");
		templates_[templateName] = temp;
	}
	else
	{
		temp = itor->second;
	}

	temp->render(data, result, baseContext);
	return true;
}

