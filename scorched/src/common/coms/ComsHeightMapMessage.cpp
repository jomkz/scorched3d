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

#include <coms/ComsHeightMapMessage.h>
#include <common/Defines.h>

ComsHeightMapMessage::ComsHeightMapMessage() :
	ComsMessage("ComsHeightMapMessage"),
	levelData_(0), levelLen_(0)
{

}

ComsHeightMapMessage::~ComsHeightMapMessage()
{
	delete [] levelData_;
	levelData_ = 0;
}

void ComsHeightMapMessage::createMessage(
	unsigned char *levelData,
	unsigned int levelLen)
{
	levelData_ = levelData;
	levelLen_ = levelLen;
}

bool ComsHeightMapMessage::writeMessage(NetBuffer &buffer)
{
	DIALOG_ASSERT(levelData_ && levelLen_);

	buffer.addToBuffer(levelLen_);
	if (levelLen_ && levelData_)
	{
		buffer.addDataToBuffer(levelData_, levelLen_);
	}
	return true;
}

bool ComsHeightMapMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(levelLen_)) return false;
	if (levelLen_)
	{
		if (levelData_) delete [] levelData_;
		levelData_ = new unsigned char[levelLen_];
		if (!reader.getDataFromBuffer(levelData_, levelLen_)) return false;
	}
	return true;
}

