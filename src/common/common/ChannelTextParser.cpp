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

#include <common/ChannelTextParser.h>
#include <engine/ScorchedContext.h>
#include <tank/TankContainer.h>

ChannelTextParser::ChannelTextParser()
{
}

ChannelTextParser::~ChannelTextParser()
{
}

ChannelTextParser::ChannelTextEntry *ChannelTextParser::getEntry(int position)
{
	if (position >= (int) entryIndex_.size()) return 0;

	unsigned int index = entryIndex_[position];
	if (index == 0) return 0;

	ChannelTextEntry &textEntry = entries_[index - 1];
	return &textEntry;
}

void ChannelTextParser::subset(ChannelTextParser &other, int start, int len)
{
	entries_ = other.entries_;

	// Create this channel text from a subset of another channel text
	for (int i=start; i<start+len; i++)
	{
		text_.append(S3D::formatStringBuffer("%c", other.text_.c_str()[i]));
		entryIndex_.push_back(other.entryIndex_[i]);
	}
}

void ChannelTextParser::addIndex(int number, unsigned char index)
{
	for (int i=0; i<number; i++)
	{
		entryIndex_.push_back(index);
	}
}

void ChannelTextParser::parseText(ScorchedContext &context, const char *text)
{
	// Clear any existing items
	text_.clear();
	entryIndex_.clear();
	entries_.clear();

	// Parse out all of the urls
	const char *pos = text;
	const char *start = 0;

	// Find the next [
	while (start = strchr(pos, '['))
	{
		// Add all text before [
		text_.append(pos, start - pos);
		addIndex(start - pos, 0);
		pos = start; // Skip to this point

		// Find the next ]
		const char *end = strchr(start, ']');
		if (end)
		{
			// Create the url
			std::string url;
			url.append(pos, end - pos + 1);

			// Parse the url
			ChannelTextEntry newEntry;
			if (parseUrl(context, url.c_str(), newEntry))
			{
				// Update the entryIndex array to point to the newEntry
				entries_.push_back(newEntry);
				addIndex((int) newEntry.text.size(), (unsigned char) entries_.size());

				// Add the new url text
				text_.append(newEntry.text);
			}
			else
			{
				// Add the normal text
				text_.append(url);
				addIndex((int) url.size(), 0);
			}

			// Skip the url
			pos = end + 1;
		}
	}

	// Add all remaining text
	text_.append(pos);
	addIndex((int) strlen(pos), 0);
}

bool ChannelTextParser::parseUrl(ScorchedContext &context, 
	const char *url, ChannelTextEntry &entry)
{
	// Find the url seperator
	const char *colon = strchr(url, ':');
	if (!colon) return false;

	// Strip the [] chars and the :
	std::string urlPart(url + 1, colon - url - 1);
	std::string otherPart(colon + 1, strlen(colon) - 2);
	entry.part = otherPart;

	// Find a url handler based on the url
	// Hardcoded for now
	if (0 == strcmp(urlPart.c_str(), "c")) // A channel
	{
		return createChannelEntry(context, otherPart.c_str(), entry);
	}
	else if (0 == strcmp(urlPart.c_str(), "p")) // A player
	{
		return createPlayerEntry(context, otherPart.c_str(), entry);
	}
	else if (0 == strcmp(urlPart.c_str(), "t")) // A tip
	{
		return createTipEntry(context, otherPart.c_str(), entry);
	}
	else if (0 == strcmp(urlPart.c_str(), "w")) // A weapon
	{
		return createWeaponEntry(context, otherPart.c_str(), entry);
	} 
	else if (0 == strcmp(urlPart.c_str(), "a")) // An admin
	{
		return createAdminEntry(context, otherPart.c_str(), entry);
	}
	return false;
}

bool ChannelTextParser::createPlayerEntry(ScorchedContext &context, 
	const char *part, ChannelTextEntry &entry)
{
	entry.type = ePlayerEntry;
	entry.text = S3D::formatStringBuffer("[%s]", part);
	entry.data = 0;

	Tank *tank = context.getTankContainer().getTankByName(part);
	if (tank)
	{
		entry.data = tank->getPlayerId();
		entry.color = tank->getColor();
	}

	return true;
}

bool ChannelTextParser::createWeaponEntry(ScorchedContext &context, 
	const char *part, ChannelTextEntry &entry)
{
	entry.type = eWeaponEntry;
	entry.text = S3D::formatStringBuffer("[%s]", part);
	entry.color = Vector(1.0f, 1.0f, 1.0f);

	return true;
}

bool ChannelTextParser::createTipEntry(ScorchedContext &context, 
	const char *part, ChannelTextEntry &entry)
{
	entry.type = eTipEntry;
	entry.text = S3D::formatStringBuffer("%s", part);
	entry.color = Vector(1.0f, 1.0f, 0.0f);

	return true;
}

bool ChannelTextParser::createChannelEntry(ScorchedContext &context, 
	const char *part, ChannelTextEntry &entry)
{
	entry.type = eChannelEntry;
	entry.text = S3D::formatStringBuffer("[%s]", part);

	return true;
}

bool ChannelTextParser::createAdminEntry(ScorchedContext &context, 
	const char *part, ChannelTextEntry &entry)
{
	entry.type = eAdminEntry;
	entry.text = S3D::formatStringBuffer("%s (Admin)", part);
	entry.color = Vector(1.0f, 1.0f, 1.0f);

	return true;
}
