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

#include <net/NetServerUDP.h>
#include <net/NetMessagePool.h>
#include <net/NetOptions.h>
#include <common/Logger.h>

//#define UDP_TEST

NetServerUDPDestination::NetServerUDPDestination(NetServerUDP *server, IPaddress &address) :
	server_(server),
	sendSeq_(1), recvSeq_(1), recvMessage_(0),
	packetLogging_(false),
	packetsSent_(0), packetsWaiting_(0), droppedPackets_(0), 
	packetsAcked_(0), 
	packetsRecieved_(0), packetsAsyncRecieved_(0),
	packetsOutOfSequenceRecieved_(0), packetsSequenceRecieved_(0), packetsDuplicateRecieved_(0),
	messagesSent_(0), messagesRecieved_(0)
{
	packetLogging_ = NetOptions::instance()->getPacketLogging();
	memcpy(&address_, &address, sizeof(address));
}

NetServerUDPDestination::~NetServerUDPDestination()
{
	// Tidy the currently recieving message
	if (recvMessage_) NetMessagePool::instance()->addToPool(recvMessage_);
	recvMessage_ = 0;

	// Tidy the current list of outgoing messages
	while (!outgoingMessages_.empty())
	{
		OutgoingMessage *message = outgoingMessages_.front();
		outgoingMessages_.pop_front();
		NetMessagePool::instance()->addToPool(message->message_);
		delete message;
	}
	while (!outgoingAsyncMessages_.empty())
	{
		OutgoingMessage *message = outgoingAsyncMessages_.front();
		outgoingAsyncMessages_.pop_front();
		NetMessagePool::instance()->addToPool(message->message_);
		delete message;
	}

	// Tidy the current list of incoming messages
	std::map<unsigned int, NetMessage *>::iterator itor;
	for (itor = incomingMessages_.begin();
		itor != incomingMessages_.end();
		itor++)
	{
		NetMessagePool::instance()->addToPool((*itor).second);
	}
}

void NetServerUDPDestination::processData(unsigned int destinationId, int len, unsigned char *data, bool fin)
{
	if (len<6)
	{
		Logger::log(S3D::formatStringBuffer("NetServerUDP: Small packet encountered, size %i", len));
		return;
	}

	// We've got new data, remove the headers (packet type and sequence no)
	data++; len--;// Remove type
	Uint32 seqValue = 0;
	memcpy(&seqValue, data, sizeof(seqValue));
	unsigned int seq = SDLNet_Read32(&seqValue);
    data+=4; len-=4;// Remove seq
	packetsRecieved_++;

	if (packetLogging_)
	{
		Logger::log(S3D::formatStringBuffer("Recieving part %u%s - %i bytes", seq, (fin?"*":" "), len));
	}

	if (seq == 0) // Is this async
	{
		// We should have all the information we need in this 1 packet
		NetMessage *recvMessage = NetMessagePool::instance()->
			getFromPool(NetMessage::BufferMessage,
				destinationId, getIpAddress());
		recvMessage->getBuffer().addDataToBuffer(data, len);
		server_->incomingMessageHandler_.addMessage(recvMessage);
		packetsAsyncRecieved_++;
	}
	else if (seq == recvSeq_) // Is this is next packet we want
	{
		// We've recieved the next message in the sequence
		// add the data to the current buffer
		packetsSequenceRecieved_++;
		addData(destinationId, len, data, fin);
		recvSeq_++;

		// Check if we already have recieved the parts in the sequence after this one 
		std::map<unsigned int, NetMessage *>::iterator finditor;
		while ((finditor = incomingMessages_.find(recvSeq_)) != incomingMessages_.end())
		{
			// For each one we have, add it to the current buffer
			NetMessage *message = (*finditor).second;
			incomingMessages_.erase(finditor);

			addData(destinationId, 
				(int) message->getBuffer().getBufferUsed(), 
				(unsigned char *) message->getBuffer().getBuffer(), 
				(message->getMessageType() == NetMessage::DisconnectMessage));

			NetMessagePool::instance()->addToPool(message);

			recvSeq_++;
		}
	}
	else if (seq > recvSeq_) 
	{
		// We've recieved a message later on in the sequence
		// Check we have not already got this message
		if (incomingMessages_.find(seq) == incomingMessages_.end())
		{
			packetsOutOfSequenceRecieved_++;

			// We don't have this packet yet, add the data
			NetMessage *message = NetMessagePool::instance()->
				getFromPool((fin?NetMessage::DisconnectMessage:NetMessage::BufferMessage),
					destinationId, getIpAddress());
			message->getBuffer().addDataToBuffer(data, len);
			incomingMessages_[seq] = message;
		}
		else
		{
			packetsDuplicateRecieved_++;
		}
	}

	bool sendAck = true;
	if (seq == 0)
	{
		// Its async no ack required
		sendAck = false;
	}
#ifdef UDP_TEST
	// Test code to simulate packet loss
	if (rand() < RAND_MAX / 4)
	{
		Logger::log("WARNING: UDP Test Enabled - Drop");
		sendAck = false;
	}
#endif // UDP_TEST

	if (sendAck)
	{
		// Send an ack
		if (packetLogging_)
		{
			Logger::log(S3D::formatStringBuffer("Sending ack %u", seq));
		}
		server_->packetVOut_[0]->len = 5;
		server_->packetVOut_[0]->address.host = address_.host;
		server_->packetVOut_[0]->address.port = address_.port;
		server_->packetVOut_[0]->data[0] = server_->eDataAck;
		server_->packetVOut_[0]->channel = -1;
		SDLNet_Write32(seq, &server_->packetVOut_[0]->data[1]);
		if (SDLNet_UDP_SendV(server_->udpsock_, server_->packetVOut_, 1) == 0)
		{
			Logger::log(S3D::formatStringBuffer("NetServerUDP: Failed to send ack packet"));
		}
	}
}

void NetServerUDPDestination::addData(unsigned int destinationId, int len, unsigned char *data, bool fin)
{
	// If this is a new message, create it
	if (!recvMessage_)
	{
		recvMessage_ = NetMessagePool::instance()->
			getFromPool(NetMessage::BufferMessage,
				destinationId, getIpAddress());
	}

	// Add the data
	recvMessage_->getBuffer().addDataToBuffer(data, len);

	// If this is a finished message, complete it
	if (fin)
	{
		server_->incomingMessageHandler_.addMessage(recvMessage_);
		recvMessage_ = 0;
		messagesRecieved_++;
	}
}

void NetServerUDPDestination::processDataAck(unsigned int destinationId, int len, unsigned char *data)
{
	// Get the sequence number
	data++; len--;// Remove type
	Uint32 seqValue = 0;
	memcpy(&seqValue, data, sizeof(seqValue));
	unsigned int seq = SDLNet_Read32(&seqValue);

	if (packetLogging_)
	{
		Logger::log(S3D::formatStringBuffer("Recieving ack %u", seq));
	}

	// Process this ack
	processAck(seq);
}

void NetServerUDPDestination::processAck(unsigned int seq)
{
	if (outgoingMessages_.empty()) return;

	// Remove this packet from the list of sent packets
	OutgoingMessage *message = outgoingMessages_.front();
	std::map<unsigned int, MessagePart>::iterator findItor = 
		message->sentParts_.find(seq);
	if (findItor != message->sentParts_.end())
	{
		unsigned int theTime = SDL_GetTicks();
		MessagePart &part = (*findItor).second;
		packetTime_ = theTime - part.sendtime;
		packetsWaiting_--; // One packet sent and acked
		packetsAcked_++;

		message->sentParts_.erase(findItor);
	}

	// Check if we have finished sending this message
	if (message->pendingParts_.empty() &&
		message->sentParts_.empty())
	{
		messagesSent_++;

		// Inform the client that this message has been fully sent
		server_->incomingMessageHandler_.addMessage(message->message_);
		delete message;
		outgoingMessages_.pop_front();
	}
}

NetServerUDPDestination::OutgoingResult NetServerUDPDestination::checkOutgoing()
{
	checkOutgoingAsync();
	return checkOutgoingSync();
}

void NetServerUDPDestination::checkOutgoingAsync()
{
	// Check we have stuff to send
	while (!outgoingAsyncMessages_.empty())
	{
		OutgoingMessage *message = outgoingAsyncMessages_.front();
		outgoingAsyncMessages_.pop_front();

		// Get the next part to send
		MessagePart part = message->pendingParts_.front();

		// Send this part
		if (sendPart(part, *message->message_))
		{
			message->pendingParts_.pop_front();
			message->sentParts_[part.seq] = part;
		}

		DIALOG_ASSERT(part.seq == 0);

		messagesSent_++;
		packetsWaiting_--; 
		packetsAcked_++;

		// Inform the client that this message has been fully sent
		server_->incomingMessageHandler_.addMessage(message->message_);
		delete message;
	}
}

NetServerUDPDestination::OutgoingResult NetServerUDPDestination::checkOutgoingSync()
{
	OutgoingResult result = OutgoingEmpty;

	// Check we have stuff to send
	if (outgoingMessages_.empty()) return result;

	const unsigned int MaximumOutstandingParts = 
		(unsigned int) NetOptions::instance()->getOutstandingPackets();
	// Send more parts if
	// 1) There are more parts to send
	// 2) We dont have more than MaximumOutstandingParts parts
	OutgoingMessage *message = outgoingMessages_.front();
	while (!message->pendingParts_.empty() &&
		message->sentParts_.size() < MaximumOutstandingParts)
	{
		result = OutgoingSent;

		// Get the next part to send
		MessagePart part = message->pendingParts_.front();

		// Send this part
		if (sendPart(part, *message->message_))
		{
			message->pendingParts_.pop_front();
			message->sentParts_[part.seq] = part;
		}

		DIALOG_ASSERT(part.seq != 0);
	}

	// Check for any out of date parts
	// Any parts we have sent and not recieved an ack for
	if (!message->sentParts_.empty())
	{
		// Iterate through all of the parts we have sent
		// and have no ack for
		unsigned int theTime = SDL_GetTicks();
		std::map<unsigned int, MessagePart>::iterator itor;
		for (itor = message->sentParts_.begin();
			itor != message->sentParts_.end();
			itor++)
		{
			// Check if this part was sent longer than the timeout
			// period ago
			MessagePart &part = (*itor).second;
			if (theTime - part.sendtime > ((25 * part.retries) + 100))
			{
				// If it was then this is a dropped part (or ack)
				droppedPackets_++;
				part.retries ++;

				// Check if we have exceeded maximum number of part retries
				unsigned int MaxNumberOfRetries = 
					(unsigned int) NetOptions::instance()->getNumberRetries();
				if (part.retries >= MaxNumberOfRetries)
				{
					// We have, its a timeout
					return OutgoingTimeout;
				}
				else
				{
					// We have not, resent the part
					sendPart(part, *message->message_);
				}
			}
			else break;
		}
	}

	return result;
}

void NetServerUDPDestination::addMessage(NetMessage &oldmessage)
{
	// Add a whole new buffer to send to this destination

	// Get a new buffer from the pool
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::SentMessage, 
			oldmessage.getDestinationId(), getIpAddress());

	// Add message to new buffer
	NetBuffer &buffer = oldmessage.getBuffer();
	message->getBuffer().allocate(buffer.getBufferUsed());
	memcpy(message->getBuffer().getBuffer(), 
		buffer.getBuffer(), buffer.getBufferUsed());
	message->getBuffer().setBufferUsed(buffer.getBufferUsed());

	// Split this message into parts that will be sent
	const int SendLength = NetOptions::instance()->getPacketSize();
	OutgoingMessage *outgoingMessage = new OutgoingMessage();
	outgoingMessage->message_ = message;

	// Check how many parts
	// Remainder is the amount to send in last part
	int parts = message->getBuffer().getBufferUsed() / SendLength;
	int remainder = message->getBuffer().getBufferUsed() % SendLength;
	if (remainder > 0) parts ++;
	else remainder = SendLength; 

	// If we are sending an async message, make sure that it only has one part
	if (oldmessage.getFlags() & NetInterfaceFlags::fAsync)
	{
		if (parts > 1)
		{
			Logger::log("NetServerUDP: Cannot send async multi part message");
			return;
		}
	}

	// Add each part into the list of parts pending to send to this client
	for (int p=0; p<parts; p++)
	{
		packetsWaiting_++; // One more packet to be sent

		MessagePart part;
		part.end = (p == parts - 1);
		part.length = ((p == parts - 1)?remainder:SendLength);
		part.offset = p * SendLength;
		part.retries = 0;

		// If this message is async, then don't add the sequence number
		if (oldmessage.getFlags() & NetInterfaceFlags::fAsync)
		{
			part.seq = 0;
		}
		else
		{
			part.seq = sendSeq_++;
		}

#ifdef UDP_TEST
		// Test code for sending out of sequence packets
		if (rand() > RAND_MAX / 2)
		{
			outgoingMessage->pendingParts_.push_back(part);
		}
		else
		{
			Logger::log("WARNING: UDP Test Enabled - Out of sequence");
			outgoingMessage->pendingParts_.push_front(part);
		}
#else // UDP_TEST
		// The "normal" code to send sequenced packets
		outgoingMessage->pendingParts_.push_back(part);
#endif // UDP_TEST
	}

	if (oldmessage.getFlags() & NetInterfaceFlags::fAsync)
	{
		outgoingAsyncMessages_.push_back(outgoingMessage);
	}
	else
	{
		outgoingMessages_.push_back(outgoingMessage);
	}
}

bool NetServerUDPDestination::sendPart(MessagePart &part, NetMessage &message)
{
	// Actually send a part in a UDP packet
	packetsSent_++;

	// Update the last time this part was sent
	part.sendtime = SDL_GetTicks();

	bool sendPacket = true;
#ifdef UDP_TEST
	// Simulate packet loss from send
	if (rand() > RAND_MAX / 2)
	{
		sendPacket = false;
	}
#endif

	if (sendPacket)
	{
		// Send this part
		server_->packetVOut_[0]->len = 5 + part.length;
		server_->packetVOut_[0]->address.host = address_.host;
		server_->packetVOut_[0]->address.port = address_.port;
		server_->packetVOut_[0]->channel = -1;
		server_->packetVOut_[0]->data[0] = (part.end?server_->eDataFin:server_->eData);
		SDLNet_Write32(part.seq, &server_->packetVOut_[0]->data[1]);
		memcpy(&server_->packetVOut_[0]->data[5], &message.getBuffer().getBuffer()[part.offset], part.length);
		if (SDLNet_UDP_SendV(server_->udpsock_, server_->packetVOut_, 1) == 0)
		{
			Logger::log(S3D::formatStringBuffer("NetServerUDP: Failed to send part packet"));
			return false;
		}

		if (packetLogging_)
		{
			Logger::log(S3D::formatStringBuffer("Sending part %u%s - %i bytes, %i offset", 
				part.seq, (part.end?"*":" "), part.length, part.offset));
		}
	}
	
	return true;
}

unsigned int NetServerUDPDestination::getIpAddress()
{
	unsigned int addr = SDLNet_Read32(&address_.host);
	return addr;
}

void NetServerUDPDestination::printStats(unsigned int destination)
{
	Logger::log(S3D::formatStringBuffer("UDP Destination %u", destination));
	Logger::log(S3D::formatStringBuffer("  %u messages sent, %u messages recieved",
		messagesSent_, messagesRecieved_));
	Logger::log(S3D::formatStringBuffer("  %u sent, %u acked, %u waiting, %u dropped, %u time",
		packetsSent_, packetsAcked_, packetsWaiting_, 
		droppedPackets_, packetTime_));
	Logger::log(S3D::formatStringBuffer("  %u recieved, %u sequence, %u noseq, %u duplicate",
		packetsRecieved_, packetsSequenceRecieved_, packetsOutOfSequenceRecieved_, 
		packetsDuplicateRecieved_));
}
