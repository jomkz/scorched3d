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

#if !defined(__INCLUDE_TemplateProviderh_INCLUDE__)
#define __INCLUDE_TemplateProviderh_INCLUDE__

#include <list>
#include <string>
#include <map>

class TemplateProvider;
class TemplateData
{
public:
	TemplateData();
	virtual ~TemplateData();

	void addTmpValue(TemplateProvider *value);

private:
	std::list<TemplateProvider *> tmpValues_;
};

class TemplateProvider
{
public:
	TemplateProvider();
	virtual ~TemplateProvider();

	virtual void getStringProperty(TemplateData &data, std::string &result) = 0;
	virtual void getListProperty(TemplateData &data, std::list<TemplateProvider *> &result) = 0;
	virtual TemplateProvider *getChild(TemplateData &data, const std::string &name) = 0;
};

class TemplateProviderString : public TemplateProvider
{
public:
	TemplateProviderString(const std::string &value) : value_(value) {}
	virtual ~TemplateProviderString() {}

	virtual void getStringProperty(TemplateData &data, std::string &result) { result = value_; }
	virtual void getListProperty(TemplateData &data, std::list<TemplateProvider *> &result) { }
	virtual TemplateProvider *getChild(TemplateData &data, const std::string &name) { return 0; }

	static TemplateProviderString *getStaticValue(const std::string &value);
protected:
	std::string value_;
};

class TemplateProviderList : public TemplateProvider
{
public:
	TemplateProviderList(const std::list<TemplateProvider *> &value) : value_(value) {}
	virtual ~TemplateProviderList() {}

	virtual void getStringProperty(TemplateData &data, std::string &result) { result = "<ListValue>"; }
	virtual void getListProperty(TemplateData &data, std::list<TemplateProvider *> &result) { result = value_; }
	virtual TemplateProvider *getChild(TemplateData &data, const std::string &name) { return 0; }

protected:
	std::list<TemplateProvider *> value_;
};

class TemplateProviderLocal : public TemplateProvider
{
public:
	TemplateProviderLocal();
	TemplateProviderLocal(TemplateProvider *parent);
	virtual ~TemplateProviderLocal();

	virtual void getStringProperty(TemplateData &data, std::string &result);
	virtual void getListProperty(TemplateData &data, std::list<TemplateProvider *> &result);
	virtual TemplateProvider *getChild(TemplateData &data, const std::string &name);

	void addLocalVariable(const std::string &name, TemplateProvider *value);

protected:
	TemplateProvider *parent_;
	std::map<std::string, TemplateProvider *> localVariables_;
};


#endif // __INCLUDE_TemplateProviderh_INCLUDE__
