////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#if !defined(__INCLUDE_TankModh_INCLUDE__)
#define __INCLUDE_TankModh_INCLUDE__

#include <list>

struct ModIdentifierEntry;
class TankMod
{
public:
	TankMod();
	virtual ~TankMod();

	void setReadyToReceive(bool r) { readyToReceive_ = r; }
	bool getReadyToReceive() { return readyToReceive_; }

	void setInit(bool i) { init_ = i; }
	bool getInit() { return init_; }

	void setSent(bool s) { sent_ = s; }
	bool getSent() { return sent_; }

	void setTotalLeft(unsigned int size) { totalLeft_ = size; };
	unsigned int getTotalLeft() { return totalLeft_; }

	void addFile(ModIdentifierEntry &entry);
	void rmFile(const char *file);
	ModIdentifierEntry *getFile(const char *file);
	std::list<ModIdentifierEntry> &getFiles() { return *files_; }

protected:
	bool readyToReceive_;
	bool sent_;
	bool init_;
	unsigned int totalLeft_;
	std::list<ModIdentifierEntry> *files_; // Ptr so we dont include ModFiles.h

};

#endif
