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

#ifndef _SoundBufferOV_H_
#define _SoundBufferOV_H_

#ifdef HAVE_OGG

#include <sound/SoundBuffer.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define OGG_BUFFER_SIZE (1024 * 1024)

class SoundBufferOV : public SoundBuffer
{
public:
	SoundBufferOV(const char *wavFileName);
	virtual ~SoundBufferOV();

	SoundBufferSourceInstance *createSourceInstance(unsigned int source);

	static bool openStream(const char *fileName, OggVorbis_File &oggStream);
	static int readData(OggVorbis_File &oggStream, char *data, int dataSize);
	static bool addDataToBuffer(vorbis_info *vorbisInfo, int buffer, char *dataBuffer, int size);

	static const char *errorString(int code);
	static size_t read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
	static int seek_func(void *datasource, ogg_int64_t offset, int whence);
	static int close_func(void *datasource);
	static long tell_func(void *datasource);

protected:
	unsigned int buffer_;
};

#endif // HAVE_OGG

#endif /* _SoundBufferOV_H_ */
