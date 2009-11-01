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

#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <net/NetBuffer.h>
#include <stdlib.h>
#include <stdio.h>

RandomGenerator::~RandomGenerator()
{
}

// Yes you guessed it, a really sucky way to create cross platform
// random numbers.  Store them in a file and read them in each system!

unsigned int FileRandomGenerator::bufferSize_ = 0;
unsigned int *FileRandomGenerator::buffer_ = 0;

FileRandomGenerator::FileRandomGenerator() :
	position_(0)
{
	// Cache the buffer so we only read it once
	// We can create many random generators easily now
	if (!buffer_)
	{
		std::string fileName = S3D::getDataFile("data/random.no");
		FILE *in = fopen(fileName.c_str(), "rb");
		DIALOG_ASSERT(in);
		bufferSize_= 100000;
		unsigned int *tmpbuffer = new unsigned int[bufferSize_];
		int size = (int) fread(tmpbuffer, sizeof(unsigned int), bufferSize_, in);
		fclose(in);	
		DIALOG_ASSERT(size == bufferSize_);

		buffer_ = new unsigned int[bufferSize_];
		for (unsigned int i=0; i<bufferSize_; i++)
		{
			unsigned int value = tmpbuffer[i];
			buffer_[i] = SDLNet_Read32(&value);
		}
		delete [] tmpbuffer;
	}
}

FileRandomGenerator::~FileRandomGenerator()
{
}

void FileRandomGenerator::seed(unsigned int seed)
{
	position_ = seed;
}

unsigned int FileRandomGenerator::getSeed()
{
	return position_;
}

unsigned int FileRandomGenerator::getRandUInt()
{
	unsigned int pos = position_ % bufferSize_;
	position_++;

	unsigned int maskpos = (position_ / bufferSize_) % 32;
	unsigned int value = buffer_[pos];
	unsigned int lvalue = value << (32 - maskpos);
	unsigned int rvalue = value >> maskpos;
	return lvalue | rvalue;
}

fixed FileRandomGenerator::getRandFixed()
{
	unsigned int rd = getRandUInt();
	unsigned int fract = rd % 10000;
	fixed result(true, fract);

	return result;
}

bool FileRandomGenerator::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(position_);
	return true;
}

bool FileRandomGenerator::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(position_)) return false;
	return true;
}