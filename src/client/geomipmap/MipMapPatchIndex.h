////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_MipMapPatchIndexh_INCLUDE__)
#define __INCLUDE_MipMapPatchIndexh_INCLUDE__

class GLVertexBufferObject;
class MipMapPatchIndex
{
public:
	MipMapPatchIndex();
	~MipMapPatchIndex();

	enum Borders
	{
		BorderLeft =   0x00000007,
		BorderRight =  0x00000038,
		BorderTop =    0x000001C0,
		BorderBottom = 0x00000E00
	};

	void generate(int size, int totalsize, int skip, unsigned int border);

	unsigned short *getIndices() { return indices_; }
	int getSize() { return size_; }
	GLVertexBufferObject *getBufferObject() { return bufferObject_; }

protected:
	int size_;
	unsigned short *indices_;
	GLVertexBufferObject *bufferObject_;
};

#endif // __INCLUDE_MipMapPatchIndexh_INCLUDE__
