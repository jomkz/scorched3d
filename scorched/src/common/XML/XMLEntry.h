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

#if !defined(__INCLUDE_XMLEntryh_INCLUDE__)
#define __INCLUDE_XMLEntryh_INCLUDE__

#include <XML/XMLParser.h>

class XMLEntry
{
public:
	XMLEntry();
	virtual ~XMLEntry();

	virtual bool readXML(XMLNode *parentNode) = 0;
	virtual void writeXML(XMLNode *parentNode) = 0;
protected:
};

class XMLEntryContainer : public XMLEntry
{
public:
	XMLEntryContainer();
	virtual ~XMLEntryContainer();

	void addChildXMLEntry(XMLEntry *entry);

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode);
	virtual void writeXML(XMLNode *parentNode);

private:
	std::list<XMLEntry *> xmlEntryChildren_;
};

class XMLEntryGroup : public XMLEntryContainer
{
public:
	XMLEntryGroup(const std::string &name, const std::string &description);
	virtual ~XMLEntryGroup();

	// XMLEntry
	virtual bool readXML(XMLNode *parentNode);
	virtual void writeXML(XMLNode *parentNode);

private:
	std::string xmlEntryName_, xmlEntryDescription_;
};

#endif
