////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#ifndef _GLFONT2DSTORAGE_H_
#define _GLFONT2DSTORAGE_H_

#include <GLEXT/GLState.h>

class GLFont2dStorage
{
public:
	struct CharEntry 
	{
	public:
		CharEntry();
		~CharEntry();

		float x, y;
		float width, height;
		int advances;
		int left, rows;

		GLuint texture;
		GLuint displaylist;
	};

	GLFont2dStorage();
	~GLFont2dStorage();

	CharEntry *getEntry(unsigned int character);
	static unsigned int getTotalCharacterBlocks() { return totalCharacterBlocks_; }

protected:
	static unsigned int totalCharacterBlocks_;
	struct StorageBlock
	{
	public:
		StorageBlock();
		~StorageBlock();

		CharEntry *entries;
	};

	StorageBlock **blocks_;
};

#endif /* _GLFONT2DSTORAGE_H_ */
