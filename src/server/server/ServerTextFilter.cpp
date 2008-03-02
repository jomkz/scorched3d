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

#include <vector>
#include <server/ServerTextFilter.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>

#define IS_SPACE(c) ((c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z'))

ServerTextFilter::ServerTextFilter() : lastReadTime_(0)
{
}

ServerTextFilter::~ServerTextFilter()
{
}

void ServerTextFilter::filterString(std::string &inputText)
{
	loadFile();

	if (words_.empty()) return;
	std::vector<TextPart> parts;

	// Split the string into parts (words) each seperated by the 
	// seperating characters
    std::string current;
	const char *pos = 0;
	for (const char *c = inputText.c_str(); *c; c++)
	{
		if (IS_SPACE(*c))
		{
			if (current.c_str()[0])
			{
				TextPart part;
				part.part = current;
				part.pos = (char *) pos;
				parts.push_back(part);
			}
			current = "";
			pos = 0;
		}
		else
		{
			if (!pos) pos = c;
			current += *c;
		}
	}
	// The first/last part
	if (current.c_str()[0])
	{
		TextPart part;
		part.part = current;
		part.pos = (char *) pos;
		parts.push_back(part);
	}

	// For each part 
	std::vector<TextPart>::iterator itor;
	for (itor = parts.begin(); itor != parts.end(); itor++)
	{
		TextPart &part = (*itor);
		const char *text = part.part.c_str();
		
		// Check that they don't contain the words
		std::list<std::string>::iterator witor;
		for (witor = words_.begin(); witor != words_.end(); witor++)
		{
			const char *word = (*witor).c_str();
			char *pos = S3D::stristr(text, word);
			if (pos)
			{
				// Only filter out if the words is at the start or end of the word
				if (pos == text || (pos - text) + strlen(word) == strlen(text))
				{
					// If they do then * out the word
					for (int i=0; i<(int) strlen(word); i++)
					{
						pos[i] = '*';
					}
				}
			}
		}
	}

	// For each combination of parts check that they don't add up to the words
	for (int i=0; i<(int) parts.size(); i++)
	{
		std::string sofar;
		for (int j=i; j<(int) parts.size(); j++)
		{
			sofar += parts[j].part;
			const char *text = sofar.c_str();

			// Check each word against the parts so far
			std::list<std::string>::iterator witor;
			for (witor = words_.begin(); witor != words_.end(); witor++)
			{
				const char *word = (*witor).c_str();
				if (stricmp(text, word) == 0)
				{
					// If they match, * out all parts
					for (int k=i; k<=j; k++)
					{
						for (char *pos = (char *) parts[k].part.c_str();
							*pos; 
							pos++)
						{
							*pos = '*';
						}
					}

					break;
				}
			}
		}
	}

	// Re-form the words
	for (itor = parts.begin(); itor != parts.end(); itor++)
	{
		TextPart &part = (*itor);
		const char *text = part.part.c_str();

		for (int i=0; i<(int) part.part.size(); i++)
		{
			part.pos[i] = text[i];
		}
	}
}

void ServerTextFilter::loadFile()
{
	std::string filename = 
		S3D::getSettingsFile(S3D::formatStringBuffer("filter-%i.txt", 
			ScorchedServer::instance()->getOptionsGame().getPortNo()));
	if (!::S3D::fileExists(filename)) return;

	time_t fileTime = S3D::fileModTime(filename);
	if (fileTime == lastReadTime_) return;

    FileLines lines;
	if (!lines.readFile(filename)) return;

	lastReadTime_ = fileTime;

	words_.clear();
	std::vector<std::string>::iterator itor;
	for (itor = lines.getLines().begin();
		itor != lines.getLines().end();
		itor++)
	{
		if ((*itor).c_str()[0])
		{
			words_.push_back(*itor);
		}
	}
}
