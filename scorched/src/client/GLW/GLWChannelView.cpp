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

#include <GLW/GLWChannelView.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWColors.h>
#include <GLEXT/GLState.h>
#include <graph/OptionsDisplay.h>
#include <image/ImageFactory.h>
#include <XML/XMLParser.h>
#include <common/Keyboard.h>
#include <common/ToolTipResource.h>
#include <client/ClientChannelManager.h>
#include <client/ScorchedClient.h>
#include <sound/SoundUtils.h>
#include <tank/TankContainer.h>
#include <lang/LangResource.h>
#include <lang/LangParam.h>

GLWChannelViewI::~GLWChannelViewI()
{
}

REGISTER_CLASS_SOURCE(GLWChannelView);

GLWChannelView::GLWChannelView() : 
	init_(false), 
	visibleLines_(5), totalLines_(50),
	displayTime_(10.0f),
	fontSize_(12.0f), outlineFontSize_(14.0f), lineDepth_(18),
	currentVisible_(0), lastChannelId_(1), lastWhisperSrc_(0),
	alignTop_(false), parentSized_(false), splitLargeLines_(false),
	scrollPosition_(-1), allowScroll_(false), 
	showChannelName_(true), showChannelNumber_(true),
	upButton_(x_ + 2.0f, y_ + 1.0f, 12.0f, 12.0f),
	downButton_(x_ + 2.0f, y_ + 1.0f, 12.0f, 12.0f),
	resetButton_(x_ + 2.0f, y_ + 1.0f, 14.0f, 14.0f),
	scrollUpKey_(0), scrollDownKey_(0), scrollResetKey_(0),
	handler_(0),
	createdTexture_(false)
{
	upButton_.setHandler(this);
	downButton_.setHandler(this);
	resetButton_.setHandler(this);

	upButton_.setToolTip(new ToolTipResource(
		ToolTip::ToolTipAlignLeft | ToolTip::ToolTipHelp, 
		"CHAT_PREVIOUS", "Chat Previous", 
		"CHAT_PREVIOUS_TOOLTIP", "Show previous chat entry"));
	downButton_.setToolTip(new ToolTipResource(
		ToolTip::ToolTipAlignLeft | ToolTip::ToolTipHelp, 
		"CHAT_NEXT", "Chat Next", 
		"CHAT_NEXT_TOOLTIP", "Show next chat entry"));
	resetButton_.setToolTip(new ToolTipResource(
		ToolTip::ToolTipAlignLeft | ToolTip::ToolTipHelp, 
		"CHAT_LAST", "Chat Last", 
		"CHAT_LAST_TOOLTIP", "View end of the chat log, \n"
		"hide all elapsed entries"));
}

GLWChannelView::~GLWChannelView()
{
}

GLWChannelView::CurrentChannelEntry *GLWChannelView::getChannel(const std::string &channelName)
{
	std::list<CurrentChannelEntry>::iterator itor;
	for (itor = currentChannels_.begin();
		itor != currentChannels_.end();
		itor++)
	{
		CurrentChannelEntry &current = *itor;
		if (channelName == current.channel) return &current;
		if (current.id == atoi(channelName.c_str())) return &current;
	}
	return 0;
}

void GLWChannelView::registeredForChannels(
	std::list<ChannelDefinition> &registeredChannels,
	std::list<ChannelDefinition> &availableChannels)
{
	// Some not efficient stuff....
	std::list<CurrentChannelEntry> oldCurrentChannels = currentChannels_;
	std::list<BaseChannelEntry> oldAvailableChannels = availableChannels_;
	currentChannels_.clear();
	availableChannels_.clear();

	// For all of the new channels
	{
		std::list<ChannelDefinition>::iterator itor;
		for (itor = registeredChannels.begin();
			itor != registeredChannels.end();
			itor++)
		{
			std::string channel = itor->getChannel();

			// Find out if we were already in this channel
			bool found = false;
			std::list<CurrentChannelEntry>::iterator olditor;
			for (olditor = oldCurrentChannels.begin();
				olditor != oldCurrentChannels.end();
				olditor++)
			{
				CurrentChannelEntry &oldEntry = (*olditor);
				if (channel == oldEntry.channel)
				{
					// Add the existing channel, under the existing number
					found = true;
					currentChannels_.push_back(oldEntry);
					break;
				}
			}
			if (!found)
			{
				// Add a new channel with a new number
				CurrentChannelEntry newEntry;
				newEntry.id = lastChannelId_++;
				newEntry.color = Vector(0.8f, 0.8f, 0.8f);
				newEntry.channel = channel;
				newEntry.type = itor->getType();

				std::map<std::string, Vector>::iterator colorItor =
					channelColors_.find(channel.c_str());
				if (colorItor != channelColors_.end()) 
				{
					newEntry.color = colorItor->second;
				}

				currentChannels_.push_back(newEntry);
			}	
		}
	}

	// For all of the available channels
	{
		std::list<ChannelDefinition>::iterator itor;
		for (itor = availableChannels.begin();
			itor != availableChannels.end();
			itor++)
		{
			std::string channel = itor->getChannel();

			// Add this channel to list of available
			BaseChannelEntry newEntry;
			newEntry.channel = channel;
			newEntry.type = itor->getType();
			availableChannels_.push_back(newEntry);
		}
	}

	if (handler_) handler_->channelsChanged(getId());
}

void GLWChannelView::joinChannel(const std::string &channelName)
{
	std::list<std::string> channels;
	formCurrentChannelList(channels);
	channels.push_back(channelName);

	ClientChannelManager::instance()->changeRegistration(this, channels);
}

void GLWChannelView::leaveChannel(const std::string &channelName)
{
	std::list<std::string> channels;
	formCurrentChannelList(channels);
	channels.remove(channelName);

	ClientChannelManager::instance()->changeRegistration(this, channels);
}

void GLWChannelView::channelText(ChannelText &channelText)
{
	if (!OptionsDisplay::instance()->getNoChannelTextSound())
	{
		if (!(channelText.getFlags() & ChannelText::eNoSound) &&
			!textSound_.empty())
		{
			CACHE_SOUND(sound, S3D::getModFile(textSound_.c_str()));
			SoundUtils::playRelativeSound(VirtualSoundPriority::eText, sound);	
		}
	}

	CurrentChannelEntry *channel = getChannel(channelText.getChannel());
	if (!channel) return;

	Tank *tank = ScorchedClient::instance()->getTankContainer().
		getTankById(channelText.getSrcPlayerId());
	if (tank)
	{
		if (channel->type & ChannelDefinition::eWhisperChannel)
		{
			if (tank != ScorchedClient::instance()->getTankContainer().getCurrentTank())
			{
				lastWhisperSrc_ = tank->getPlayerId();
			}
		}
	}

	LangString channelName;
	if (showChannelNumber_)
	{
		channelName.append(LANG_PARAM_1("{0}. ", channel->id));
	}
	if (showChannelName_)
	{
		channelName.append(LANG_PARAM_1("[c:{0}]. ", channel->channel));
	}
	if (tank)
	{
		channelName.append(LANG_PARAM_1("[p:{0}]", tank->getTargetName()));
	}
	if (channelText.getAdminPlayer()[0])
	{
		channelName.append(LANG_PARAM_1("[a:{0}]", channelText.getAdminPlayer()));
	}
	if (tank || channelText.getAdminPlayer()[0])
	{
		channelName.append(LANG_STRING(" : "));
	}
	channelName.append(channelText.getMessage());

	GLWChannelViewTextRenderer chanText(this);
	chanText.parseText(ScorchedClient::instance()->getContext(), channelName);
	
	bool firstTime = true;
	int currentLen = 0;
	const LangString &text = chanText.getString();
	int totalLen = (int) text.size();
	while (currentLen < totalLen)
	{
		// Get the next split position
		int partLen = splitLine(&text[currentLen]);
		bool nl=(text[currentLen + partLen - 1] == '\n');

		// Create the new text and add it
		GLWChannelViewTextRenderer newText(this);
		newText.subset(chanText, currentLen, (nl?partLen-1:partLen));
		addInfo(channel->color, newText);

		// Increment the current position
		currentLen += partLen;
	}
}

void GLWChannelView::buttonDown(unsigned int id)
{
	if (!allowScroll_) return;

	if (id == upButton_.getId())
	{
		scrollPosition_ ++;
		if (scrollPosition_ > (int) textLines_.size() - 1)
			scrollPosition_ = (int) textLines_.size() - 1;
	}
	else if (id == downButton_.getId())
	{
		scrollPosition_ --;
		if (scrollPosition_ < 0) scrollPosition_ = 0;
	}
	else
	{
		scrollPosition_ = -1;
	}
}

int GLWChannelView::splitLine(const LangString &message)
{
	int lastSpace = 0;
	int totalLen = (int) message.size();
	for (int len=1; len<totalLen; len++)
	{
		float width = 0.0f;
		if (splitLargeLines_)
		{
			width = GLWFont::instance()->getGameFont()->
				getWidth(outlineFontSize_, message, len);
		}

		if (width > w_)
		{
			// If there is a space within the last 15 characters split to it
			if (lastSpace && (len - lastSpace < 15)) 
			{
				return lastSpace;
			}
			else
			{
				// Else just split where we are now
				return len - 1;
			}
		}

		if (message[len] == '\n')
		{
			return len + 1;
		}
		if (message[len] == ' ')
		{
			lastSpace = len + 1;
		}
	}

	// The whole message fits, split to the end of the message
	return totalLen;
}

void GLWChannelView::addInfo(Vector &color, GLWChannelViewTextRenderer &text)
{
	GLWChannelViewEntry entry;
	entry.text = text;
	entry.timeRemaining = displayTime_;
	entry.color = color;
	textLines_.push_front(entry);

	if (scrollPosition_ > 0)
	{
		scrollPosition_ ++;
		if (scrollPosition_ > (int) textLines_.size() - 1)
			scrollPosition_ = (int) textLines_.size() - 1;
	}

	if (textLines_.size() > (unsigned int) totalLines_)
	{
		textLines_.pop_back();
	}
}

void GLWChannelView::simulate(float frameTime)
{
	currentVisible_ = 0;
	int count = 0;
	std::list<GLWChannelViewEntry>::iterator itor;
	for (itor = textLines_.begin();
		itor != textLines_.end() && count < visibleLines_;
		itor++, count++)
	{
		GLWChannelViewEntry &entry = (*itor);
		entry.timeRemaining -= frameTime;
		if (entry.timeRemaining > 0.0f) currentVisible_ ++;
	}
}

void GLWChannelView::setX(float x)
{
	GLWidget::setX(x);
	upButton_.setX(x_ + 1.0f);
	downButton_.setX(x_ + 1.0f);
	resetButton_.setX(x_ + 1.0f);
}

void GLWChannelView::setY(float y)
{
	GLWidget::setY(y);
	upButton_.setY(y_ + 36.0f);
	downButton_.setY(y_ + 24.0f);
	resetButton_.setY(y_ + 6.0f);
}

void GLWChannelView::draw()
{
	GLWidget::draw();
	if (!createdTexture_)
	{
		createdTexture_ = true;
		Image upImg = ImageFactory::loadAlphaImage(
			ImageID::eModLocation,
			"data/windows/arrow_u.png");
		Image downImg = ImageFactory::loadAlphaImage(
			ImageID::eModLocation,
			"data/windows/arrow_d.png");
		Image resetImg = ImageFactory::loadAlphaImage(
			ImageID::eModLocation,
			"data/windows/arrow_s.png");

		upTexture_.create(upImg, false);
		downTexture_.create(downImg, false);
		resetTexture_.create(resetImg, false);

		upButton_.setTexture(&upTexture_);
		downButton_.setTexture(&downTexture_);
		resetButton_.setTexture(&resetTexture_);
	}

	if (allowScroll_)
	{
		upButton_.draw();
		downButton_.draw();
		resetButton_.draw();
	}

	if (!init_)
	{
		ClientChannelManager::instance()->registerClient(this, startupChannels_);
		init_ = true;
		if (parent_ && parentSized_)
		{
			w_ = parent_->getW();
			h_ = parent_->getH();
		}
	}

	// Check if there is anything to draw
	if (textLines_.empty()) return;

	// Find the start of the area to draw to
	GLState currentStateBlend(GLState::TEXTURE_ON | 
		GLState::BLEND_ON | GLState::DEPTH_OFF);
	float start = y_ + 8.0f; //lineDepth_;
	if (alignTop_)
	{
		start = y_ + h_ - float(currentVisible_) * lineDepth_;
	}

	// Skip the lines we are not viewing
	// (Should make this more efficient)
	std::list<GLWChannelViewEntry>::iterator startingPos = textLines_.begin();
	{
		int count = 0;
		for (;
			startingPos != textLines_.end() && count < scrollPosition_;
			startingPos++, count++)
		{
		}
	}

	// Draw all the black outlines first
	// Do this in a block incase any of the outlines would over write any
	// of the other lines
	{
		int count = 0;
		std::list<GLWChannelViewEntry>::iterator itor;
		for (itor = startingPos;
			itor != textLines_.end() && count < visibleLines_;
			itor++, count++)
		{
			GLWChannelViewEntry &entry = (*itor);

			float alpha = 1.0f;
			if (scrollPosition_ < 0)
			{
				if (entry.timeRemaining <= 0.0f) continue;
				if (entry.timeRemaining < 1.0f) alpha = entry.timeRemaining;
			}

			float x = x_ + 20.0f;
			float y = start + count * lineDepth_;

			GLWFont::instance()->getGameShadowFont()->
				drawA(GLWColors::black, alpha, fontSize_,
					x - 1.0f, y + 1.0f, 0.0f, 
					entry.text.getString());
		}
	}
	// Draw the actual text
	{
		int count = 0;
		std::list<GLWChannelViewEntry>::iterator itor;
		for (itor = startingPos;
			itor != textLines_.end() && count < visibleLines_;
			itor++, count++)
		{
			GLWChannelViewEntry &entry = (*itor);

			float alpha = 1.0f;
			if (scrollPosition_ < 0)
			{
				if (entry.timeRemaining <= 0.0f) continue;
				if (entry.timeRemaining < 1.0f) alpha = entry.timeRemaining;
			}

			float x = x_ + 20.0f;
			float y = start + count * lineDepth_;

			GLWFont::instance()->getGameFont()->
				drawA(&entry.text, entry.color, alpha, fontSize_,
					x, y, 0.0f, 
					entry.text.getString());
		}
	}
}

void GLWChannelView::mouseDown(int button, float x, float y, bool &skipRest)
{
	skipRest = false;
	upButton_.mouseDown(button, x, y, skipRest);
	downButton_.mouseDown(button, x, y, skipRest);
	resetButton_.mouseDown(button, x, y, skipRest);
}

void GLWChannelView::mouseUp(int button, float x, float y, bool &skipRest)
{
	skipRest = false;
	upButton_.mouseUp(button, x, y, skipRest);
	downButton_.mouseUp(button, x, y, skipRest);
	resetButton_.mouseUp(button, x, y, skipRest);
}

void GLWChannelView::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	skipRest = false;
	upButton_.mouseDrag(button, mx, my, x, y, skipRest);
	downButton_.mouseDrag(button, mx, my, x, y, skipRest);
	resetButton_.mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWChannelView::keyDown(char *buffer, unsigned int keyState, 
	KeyboardHistory::HistoryElement *history, int hisCount, 
	bool &skipRest)
{
	if (scrollUpKey_ && scrollUpKey_->keyDown(buffer, keyState, false))
	{
		buttonDown(upButton_.getId());
		skipRest = true;
	}
	if (scrollDownKey_ && scrollDownKey_->keyDown(buffer, keyState, false))
	{
		buttonDown(downButton_.getId());
		skipRest = true;
	}
	if (scrollResetKey_ && scrollResetKey_->keyDown(buffer, keyState, false))
	{
		buttonDown(resetButton_.getId());
		skipRest = true;
	}
}

bool GLWChannelView::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;

	if (!initFromXMLInternal(node)) return false;
		
	return true;
}

void GLWChannelView::formCurrentChannelList(std::list<std::string> &result)
{
	std::list<CurrentChannelEntry>::iterator itor;
	for (itor = currentChannels_.begin();
		itor != currentChannels_.end();
		itor++)
	{
		CurrentChannelEntry &entry = *itor;
		result.push_back(entry.channel);
	}
}

bool GLWChannelView::initFromXMLInternal(XMLNode *node)
{
	if (!node->getNamedChild("displaytime", displayTime_)) return false;
	if (!node->getNamedChild("totallines", totalLines_)) return false;
	if (!node->getNamedChild("visiblelines", visibleLines_)) return false;
	if (!node->getNamedChild("linedepth", lineDepth_)) return false;
	if (!node->getNamedChild("fontsize", fontSize_)) return false;
	if (!node->getNamedChild("outlinefontsize", outlineFontSize_)) return false;
	if (!node->getNamedChild("textaligntop", alignTop_)) return false;
	if (!node->getNamedChild("parentsized", parentSized_)) return false;
	if (!node->getNamedChild("splitlargelines", splitLargeLines_)) return false;
	if (!node->getNamedChild("allowscroll", allowScroll_)) return false;
	if (!node->getNamedChild("showchannelname", showChannelName_)) return false;
	if (!node->getNamedChild("showchannelnumber", showChannelNumber_)) return false;
	
	node->getNamedChild("textsound", textSound_, false);

	std::string startupchannel;
	while (node->getNamedChild("channel", startupchannel, false))
	{
		startupChannels_.push_back(startupchannel);
	}

	XMLNode *channelcolors;
	while (node->getNamedChild("channelcolor", channelcolors, false))
	{
		std::string channel;
		Vector color;
		if (!channelcolors->getNamedChild("channel", channel)) return false;
		if (!channelcolors->getNamedChild("color", color)) return false;
		channelColors_[channel] = color;
	}

	std::string scrollUpKey, scrollDownKey, scrollResetKey;
	if (node->getNamedChild("scrollupkey", scrollUpKey, false))
	{
		scrollUpKey_ = Keyboard::instance()->getKey(scrollUpKey.c_str());
	}
	if (node->getNamedChild("scrolldownkey", scrollDownKey, false))
	{
		scrollDownKey_ = Keyboard::instance()->getKey(scrollDownKey.c_str());
	}
	if (node->getNamedChild("scrollresetkey", scrollResetKey, false))
	{
		scrollResetKey_ = Keyboard::instance()->getKey(scrollResetKey.c_str());
	}
	return true;
}
