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

class Template;
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
	std::map<std::string, Template *> templates_;
};

#endif // __INCLUDE_TemplateRendererh_INCLUDE__
