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

#include <template/TemplateProvider.h>

TemplateData::TemplateData()
{
}

TemplateData::~TemplateData()
{
	std::list<TemplateProvider *>::iterator itor = tmpValues_.begin(),
		end = tmpValues_.end();
	for (; itor != end; ++itor) delete *itor;
	tmpValues_.clear();
}

void TemplateData::addTmpValue(TemplateProvider *value)
{
	tmpValues_.push_back(value);
}

TemplateProvider::TemplateProvider()
{
}

TemplateProvider::~TemplateProvider()
{
}

TemplateProviderLocal::TemplateProviderLocal() :
	parent_(0)
{

}

TemplateProviderLocal::TemplateProviderLocal(TemplateProvider *parent) :
	parent_(parent)
{
}

TemplateProviderLocal::~TemplateProviderLocal()
{
}

void TemplateProviderLocal::getStringProperty(TemplateData &data, std::string &result)
{
	if (parent_) parent_->getStringProperty(data, result);
}

void TemplateProviderLocal::getListProperty(TemplateData &data, std::list<TemplateProvider *> &result)
{
	if (parent_) parent_->getListProperty(data, result);
}

TemplateProvider *TemplateProviderLocal::getChild(TemplateData &data, const std::string &name)
{
	std::map<std::string, TemplateProvider *>::iterator itor = localVariables_.find(name);
	if (itor == localVariables_.end())
	{
		if (parent_) return parent_->getChild(data, name);
		return 0;
	}
	else
	{
		return itor->second;
	}
}

void TemplateProviderLocal::addLocalVariable(const std::string &name, TemplateProvider *value)
{
	localVariables_[name] = value;
}

TemplateProviderString *TemplateProviderString::getStaticValue(const std::string &value)
{
	static TemplateProviderString tmp(value);
	tmp.value_ = value;
	return &tmp;
}
