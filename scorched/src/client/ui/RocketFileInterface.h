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

#ifndef RocketFileInterface_H
#define RocketFileInterface_H

#include <Rocket/Core/String.h>
#include <Rocket/Core/FileInterface.h>

class RocketFileInterface : public Rocket::Core::FileInterface
{
public:
	RocketFileInterface();
	virtual ~RocketFileInterface();

	/// Opens a file.		
	virtual Rocket::Core::FileHandle Open(const Rocket::Core::String& path);

	/// Closes a previously opened file.		
	virtual void Close(Rocket::Core::FileHandle file);

	/// Reads data from a previously opened file.		
	virtual size_t Read(void* buffer, size_t size, Rocket::Core::FileHandle file);

	/// Seeks to a point in a previously opened file.		
	virtual bool Seek(Rocket::Core::FileHandle file, long offset, int origin);

	/// Returns the current position of the file pointer.		
	virtual size_t Tell(Rocket::Core::FileHandle file);
};

#endif
