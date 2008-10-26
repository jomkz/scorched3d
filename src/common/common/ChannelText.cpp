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

#include <common/ChannelText.h>

ChannelText::ChannelText(const std::string &channel, const LangString &message) : 
	srcPlayerId_(0), destPlayerId_(0),
	channel_(channel),
	message_(message),
	flags_(0)
{
}

bool ChannelText::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(channel_);
	buffer.addToBuffer(message_);
	buffer.addToBuffer(srcPlayerId_);
	buffer.addToBuffer(destPlayerId_);
	buffer.addToBuffer(admin_);
	buffer.addToBuffer(flags_);
	return true;
}

bool ChannelText::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(channel_)) return false;
	if (!reader.getFromBuffer(message_)) return false;
	if (!reader.getFromBuffer(srcPlayerId_)) return false;
	if (!reader.getFromBuffer(destPlayerId_)) return false;
	if (!reader.getFromBuffer(admin_)) return false;
	if (!reader.getFromBuffer(flags_)) return false;
	return true;
}

ChannelDefinition::ChannelDefinition(const char *c, unsigned int t) :
	channel_(c), type_(t)
{
}

