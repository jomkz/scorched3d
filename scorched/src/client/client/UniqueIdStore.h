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

#if !defined(__INCLUDE_UniqueIdStoreh_INCLUDE__)
#define __INCLUDE_UniqueIdStoreh_INCLUDE__

#include <XML/XMLEntrySimpleTypes.h>
#include <XML/XMLEntryRoot.h>

class UniqueIdStore
{
public:
	class Entry : public XMLEntryContainer
	{
	public:
		Entry();
		virtual ~Entry();

		virtual bool readXML(XMLNode *node, void *xmlData);

		unsigned int ip;
		XMLEntryString id, published;
	};
	class EntryList : public XMLEntryList<Entry>
	{
	public:
		EntryList();
		virtual ~EntryList();

		virtual Entry *createXMLEntry(void *xmlData);
	};
	class EntryRoot : public XMLEntryRoot<XMLEntryContainer>
	{
	public:
		EntryRoot();
		virtual ~EntryRoot();

		EntryList list_;
	};

	UniqueIdStore();
	virtual ~UniqueIdStore();

	bool loadStore();
	bool saveStore();

	const char *getUniqueId(unsigned int ip);
	bool saveUniqueId(unsigned int ip, const char *id,
		const char *published);

	EntryRoot &getIds() { return ids_; }

protected:
	EntryRoot ids_;
};

#endif // __INCLUDE_UniqueIdStoreh_INCLUDE__
