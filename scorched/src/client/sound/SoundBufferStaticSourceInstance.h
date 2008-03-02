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

#ifndef _SoundBufferStaticSourceInstance_H_
#define _SoundBufferStaticSourceInstance_H_

#include <sound/SoundBuffer.h>

class SoundBufferStaticSourceInstance : public SoundBufferSourceInstance
{
public:
	SoundBufferStaticSourceInstance(
		unsigned int source, unsigned int buffer);
	virtual ~SoundBufferStaticSourceInstance();

	virtual void play(bool loop);
	virtual void stop();
	virtual void simulate(bool loop);

protected:
	unsigned int buffer_;
};

#endif /* _SoundBufferStaticSourceInstance_H_ */
