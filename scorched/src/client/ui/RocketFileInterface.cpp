////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <ui/RocketFileInterface.h>
#include <stdio.h>

RocketFileInterface::RocketFileInterface() 
{
}

RocketFileInterface::~RocketFileInterface()
{
}

// Opens a file.
Rocket::Core::FileHandle RocketFileInterface::Open(const Rocket::Core::String& path)
{
	// Attempt to open the file relative to the application's root.
	FILE* fp = fopen((path).CString(), "rb");
	if (fp != NULL)
		return (Rocket::Core::FileHandle) fp;

	// Attempt to open the file relative to the current working directory.
	fp = fopen(path.CString(), "rb");
	return (Rocket::Core::FileHandle) fp;
}

// Closes a previously opened file.
void RocketFileInterface::Close(Rocket::Core::FileHandle file)
{
	fclose((FILE*) file);
}

// Reads data from a previously opened file.
size_t RocketFileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
{
	return fread(buffer, 1, size, (FILE*) file);
}

// Seeks to a point in a previously opened file.
bool RocketFileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin)
{
	return fseek((FILE*) file, offset, origin) == 0;
}

// Returns the current position of the file pointer.
size_t RocketFileInterface::Tell(Rocket::Core::FileHandle file)
{
	return ftell((FILE*) file);
}
