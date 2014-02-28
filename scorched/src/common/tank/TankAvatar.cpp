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

#include <tank/TankAvatar.h>
#include <common/Defines.h>
#include <image/ImagePngFactory.h>
#include <stdio.h>
#include <zlib.h>

TankAvatar::TankAvatar()
{
	file_ = new NetBuffer();
}

TankAvatar::~TankAvatar()
{
	delete file_;
}

bool TankAvatar::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TankAvatar");

	buffer.addToBufferNamed("name", name_);
	buffer.addToBufferNamed("file", *file_);
	return true;
}

void TankAvatar::clear()
{
	name_ = "";
	file_->reset();
}

bool TankAvatar::readMessage(NetBufferReader &reader)
{
	NetBuffer tmpBuffer;
	std::string name;
	if (!reader.getFromBuffer(name)) return false;
	if (!reader.getFromBuffer(tmpBuffer)) return false;
	setFromBuffer(name, tmpBuffer);
	return true;
}

bool TankAvatar::loadFromFile(const std::string &fileName)
{
	FILE *in = fopen(fileName.c_str(), "rb");
	if (in)
	{
		name_ = fileName;
		NetBuffer tmpBuffer;
		unsigned char readBuf[512];
		while (unsigned int size = (unsigned int) fread(readBuf, sizeof(unsigned char), 512, in))
		{
			tmpBuffer.addDataToBuffer(readBuf, size);
		}
		fclose(in);
		return setFromBuffer(fileName, tmpBuffer);
	}
	return false;
}

unsigned int TankAvatar::getCrc()
{
	unsigned int crc =  crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (unsigned char *) 
		file_->getBuffer(), file_->getBufferUsed());	
	return crc;
}

bool TankAvatar::setFromBuffer(const std::string &fileName, NetBuffer &buffer)
{
	if (buffer.getBufferUsed() == 0) return false;

	Image png = ImagePngFactory::loadFromBuffer(buffer);
	if (!png.getBits()) return false;
	if (png.getWidth() != 32 || 
		png.getHeight() != 32) return false;

	name_ = fileName;
	file_->reset();
	file_->addDataToBuffer(buffer.getBuffer(), 
		buffer.getBufferUsed());

	return true;
}
