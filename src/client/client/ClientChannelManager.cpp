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

#include <client/ClientChannelManager.h>
#include <client/ScorchedClient.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsChannelMessage.h>
#include <coms/ComsChannelTextMessage.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <common/Logger.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <sprites/TalkRenderer.h>
#include <engine/ActionController.h>

ClientChannelManager::ChannelEntry::ChannelEntry(ClientChannelManagerI *user) :
	user_(user)
{
}

void ClientChannelManager::ChannelEntry::setChannels(std::list<ChannelDefinition> &channels)
{
	channels_.clear();
	std::list<ChannelDefinition>::iterator itor;
	for (itor = channels.begin();
		itor != channels.end();
		itor++)
	{
		channels_.insert(itor->getChannel());
	}
}

bool ClientChannelManager::ChannelEntry::hasChannel(const char *channel)
{
	return (channels_.find(channel) != channels_.end());
}

unsigned int ClientChannelManager::nextRecieverId_ = 0;

ClientChannelManager *ClientChannelManager::instance_ = 0;

ClientChannelManager *ClientChannelManager::instance()
{
	if (!instance_)
	{
		instance_ = new ClientChannelManager;
	}
	return instance_;
}

ClientChannelManager::ClientChannelManager()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsChannelMessage",
		this);
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		"ComsChannelTextMessage",
		this);

	new ConsoleRuleMethodIAdapterEx<ClientChannelManager>(
		this, &ClientChannelManager::say, "Say", 
		ConsoleUtil::formParams(
		ConsoleRuleParam("channel", ConsoleRuleTypeString),
		ConsoleRuleParam("text", ConsoleRuleTypeString)));
}

ClientChannelManager::~ClientChannelManager()
{
}

bool ClientChannelManager::registerClient(ClientChannelManagerI *reciever,
	std::list<std::string> &channels)
{
	if (getChannelEntry(reciever) > 0) return false;

	// Add a new reciever
	ChannelEntry *entry = new ChannelEntry(reciever);
	unsigned int channelId = ++nextRecieverId_;
	recievers_[channelId] = entry;

	// Send the request for a new reciever
	ComsChannelMessage message(ComsChannelMessage::eRegisterRequest, channelId);
	std::list<std::string>::iterator itor;
	for (itor = channels.begin();
		itor != channels.end();
		itor++)
	{
		ChannelDefinition entry(itor->c_str());
		message.getChannels().push_back(entry);
	}
	ComsMessageSender::sendToServer(message);

	return true;
}

bool ClientChannelManager::deregisterClient(ClientChannelManagerI *reciever)
{
	unsigned int channelId = getChannelEntry(reciever);
	if (channelId == 0) return false;

	// Remove the reciever
	delete recievers_[channelId];
	recievers_.erase(channelId);

	// Send the request to remove the reciever
	ComsChannelMessage message(ComsChannelMessage::eDeregisterRequest, channelId);
	ComsMessageSender::sendToServer(message);

	return true;
}

bool ClientChannelManager::changeRegistration(ClientChannelManagerI *reciever,
	std::list<std::string> &channels)
{
	unsigned int channelId = getChannelEntry(reciever);
	if (channelId == 0) return false;

	// Send the request to add a new channel
	ComsChannelMessage message(ComsChannelMessage::eJoinRequest, channelId);	
	std::list<std::string>::iterator itor;
	for (itor = channels.begin();
		itor != channels.end();
		itor++)
	{
		ChannelDefinition entry(itor->c_str());
		message.getChannels().push_back(entry);
	}
	ComsMessageSender::sendToServer(message);

	return true;
}

void ClientChannelManager::addChannel(const char *lookfor, const char *channel)
{
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		itor++)
	{
		ChannelEntry *r = (*itor).second;
		if (r->hasChannel(lookfor))
		{
			std::set<std::string> currentChannels = r->getChannels();
			currentChannels.insert(channel);
			std::list<std::string> channels;
			std::set<std::string>::iterator itor;
			for (itor = currentChannels.begin();
				itor != currentChannels.end();
				itor++)
			{
				channels.push_back(*itor);
			}

			changeRegistration(r->getUser(), channels);
			break;
		}
	}
}

void ClientChannelManager::removeChannel(const char *channel)
{
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		itor++)
	{
		ChannelEntry *r = (*itor).second;
		if (r->hasChannel(channel))
		{
			std::set<std::string> currentChannels = r->getChannels();
			currentChannels.erase(channel);
			std::list<std::string> channels;
			std::set<std::string>::iterator itor;
			for (itor = currentChannels.begin();
				itor != currentChannels.end();
				itor++)
			{
				channels.push_back(*itor);
			}

			changeRegistration(r->getUser(), channels);
			break;
		}
	}
}

unsigned int ClientChannelManager::getChannelEntry(ClientChannelManagerI *reciever)
{
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		itor++)
	{
		ChannelEntry *r = (*itor).second;
		if (r->getUser() == reciever) return (*itor).first;
	}
	return 0;
}

void ClientChannelManager::say(std::vector<ConsoleRuleValue> &values)
{
	ConsoleRuleValue &channelValue = values[1];
	ConsoleRuleValue &textValue = values[2];

	ChannelText message(channelValue.valueString.c_str(), 
		textValue.valueString.c_str());
	sendText(message);
}

void ClientChannelManager::sendText(const ChannelText &constText)
{
	ChannelText text = constText;

	unsigned int playerId = 
		ScorchedClient::instance()->getTankContainer().getCurrentPlayerId();
	if (!playerId)
	{
		std::map<unsigned int, Tank *> &tanks = 
			ScorchedClient::instance()->getTankContainer().getPlayingTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			if (tank->getDestinationId() == 
				ScorchedClient::instance()->getTankContainer().getCurrentDestinationId())
			{
				playerId = tank->getPlayerId();
				break;
			}
		}
	}

	text.setSrcPlayerId(playerId);
	ComsChannelTextMessage message(text);
	ComsMessageSender::sendToServer(message);
}

void ClientChannelManager::showText(const ChannelText &constText)
{
	ChannelText text = constText;

	// Add this line to the console
	if (!(text.getFlags() & ChannelText::eNoLog))
	{
		Logger::log(S3D::formatStringBuffer("[%s] : %s",
			text.getChannel(),
			text.getMessage()));
	}

	// Send to all recievers
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		itor++)
	{
		ChannelEntry *entry = (*itor).second;
		if (entry->hasChannel(text.getChannel()))
		{
			entry->getUser()->channelText(text);
		}
	}
}

bool ClientChannelManager::processMessage(
	NetMessage &netNessage,
	const char *messageType,
	NetBufferReader &reader)
{
	// Check which message we have got
	if (0 == strcmp("ComsChannelMessage", messageType))
	{
		// We have a ChannelMessage from the server
		ComsChannelMessage channelMessage;
		if (!channelMessage.readMessage(reader)) return false;

		// Get the reciever for this message
		std::map<unsigned int, ChannelEntry *>::iterator findItor =
			recievers_.find(channelMessage.getId());
		if (findItor == recievers_.end()) return true; // No reciever
		ChannelEntry *entry = (*findItor).second;
		ClientChannelManagerI *reciever = entry->getUser();

		// Check which ChannelMessage was sent
		if (channelMessage.getType() == ComsChannelMessage::eJoinRequest)
		{
			// Its a channels request
			entry->setChannels(channelMessage.getChannels());
			reciever->registeredForChannels(channelMessage.getChannels(),
				channelMessage.getAvailableChannels());
		}
	}
	else if (0 == strcmp("ComsChannelTextMessage", messageType))
	{
		// We have a ChannelTextMessage from the server
		ComsChannelTextMessage textMessage;
		if (!textMessage.readMessage(reader)) return false;

		Tank *tank = ScorchedClient::instance()->getTankContainer().getTankById(
			textMessage.getChannelText().getSrcPlayerId());
		if(tank && tank->getState().getState() == TankState::sNormal)
		{
			// put a speach bubble over the talking tank
			Vector white(1.0f, 1.0f, 1.0f);
			TalkRenderer *talk = new TalkRenderer(
				tank->getPosition().getTankTurretPosition().asVector(),
				white);
			ScorchedClient::instance()->getActionController().
				addAction(new SpriteAction(talk));
		}
		// Ignore any messages from this tank
		if (tank && tank->getState().getMuted()) return true;

		// Log this message
		if (tank)
		{
			Logger::log(S3D::formatStringBuffer("[%s][%s] : %s",
				textMessage.getChannelText().getChannel(),
				tank->getName(),
				textMessage.getChannelText().getMessage()));
		}
		else
		{
			Logger::log(S3D::formatStringBuffer("[%s] : %s",
				textMessage.getChannelText().getChannel(),
				textMessage.getChannelText().getMessage()));
		}

		// Foreach reciever
		std::list<unsigned int>::iterator itor;
		for (itor = textMessage.getIds().begin();
			itor != textMessage.getIds().end();
			itor++)
		{
			unsigned int id = (*itor);

			// Get the reciever for this message
			std::map<unsigned int, ChannelEntry *>::iterator findItor =
				recievers_.find(id);
			if (findItor != recievers_.end())
			{
				ClientChannelManagerI *reciever = (*findItor).second->getUser();
				reciever->channelText(textMessage.getChannelText());
			}
		}
	}
	else return false;

	return true;
}
