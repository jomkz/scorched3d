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

#if !defined(__INCLUDE_ModFilesh_INCLUDE__)
#define __INCLUDE_ModFilesh_INCLUDE__

#include <engine/ModFileEntry.h>
#include <common/ProgressCounter.h>
#include <map>

class ModFiles
{
public:
	ModFiles();
	virtual ~ModFiles();

	bool exportModFiles(const std::string &mod, const std::string &fileName);
	bool importModFiles(std::string &mod, const std::string &fileName);
	bool importModFiles(std::string &mod, NetBuffer &buffer);

	bool loadModFiles(const std::string &mod, bool createDir, 
		ProgressCounter *counter = 0);
	bool writeModFiles(const std::string &mod);
	void clearData();

	std::map<std::string, ModFileEntry *> &getFiles() { return files_; }
	static bool excludeFile(const std::string &file);
	static bool fileEnding(const std::string &file, const std::string &ext);

protected:
	std::map<std::string, ModFileEntry *> files_;

	bool loadModDir(const std::string &moddir, const std::string &mod,
		ProgressCounter *counter = 0);
	bool loadModFile(const std::string &fullFileName,
		const std::string &modDir, const std::string &mod);
	bool loadLocalModFile(const std::string &local, 
		const std::string &mod);

private:
	ModFiles(const ModFiles&other);
	ModFiles &operator=(ModFiles &other);

};

#endif

