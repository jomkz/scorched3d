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

#include <landscape/LandscapeSoundManager.h>
#include <landscapedef/LandscapeInclude.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeSound.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <sound/Sound.h>

LandscapeSoundManager *LandscapeSoundManager::instance_(0);

LandscapeSoundManager *LandscapeSoundManager::instance()
{
	if (!instance_) instance_ = new LandscapeSoundManager();
	return instance_;
}

LandscapeSoundManager::LandscapeSoundManager() : 
	lastTime_(0), haveSound_(false)
{
}

LandscapeSoundManager::~LandscapeSoundManager()
{
}

void LandscapeSoundManager::cleanUp()
{
	haveSound_ = false;
	std::list<LandscapeSoundManagerEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		itor++)
	{
		LandscapeSoundManagerEntry &entry = (*itor);
		delete entry.soundSource;
		entry.soundSource = 0;
	}
	entries_.clear();
}

void LandscapeSoundManager::addSounds()
{
	cleanUp();

	// Load in the per level sound
	LandscapeDefn *defn = 
		ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getDefn();
	LandscapeTex *tex = 
		ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getTex();

	haveSound_ = true;
	loadSound(tex->texDefn.includes);
	loadSound(defn->texDefn.includes);
}

void LandscapeSoundManager::loadSound(std::vector<LandscapeInclude *> &sounds)
{
	std::vector<LandscapeInclude *>::iterator itor;
	for (itor = sounds.begin();
		itor != sounds.end();
		itor++)
	{
		LandscapeInclude *sound = (*itor);

		std::vector<LandscapeSoundType *>::iterator typeItor;
		for (typeItor = sound->sounds.begin();
			typeItor != sound->sounds.end();
			typeItor ++)
		{
			LandscapeSoundType *soundType = (*typeItor);
			for (int i=0; i<soundType->position->getInitCount(); i++)
			{
				// Create the new sound entry
				LandscapeSoundManagerEntry entry;
				entry.soundType = soundType;
				entry.initData = soundType->position->getInitData(i);
				entry.timeLeft = soundType->timing->getNextEventTime();
				entry.soundSource = new VirtualSoundSource(
					VirtualSoundPriority::eEnvironment, 
					(entry.timeLeft < 0.0f), // Looping
					false);
				entries_.push_back(entry);

				// Start any looped sounds
				if (entry.timeLeft < 0.0f)
				{
					if (entry.soundType->position->setPosition(
						entry.soundSource, entry.initData))
					{
						entry.soundType->sound->play(entry.soundSource);
					}
					else
					{
						entry.removed = true;
					}
				}
			}
		}
	}
}

void LandscapeSoundManager::simulate(float frameTime)
{
	if (OptionsDisplay::instance()->getAmbientSoundVolume() == 0)
	{
		if (haveSound_) cleanUp();
		return;
	}
	else if (!haveSound_)
	{
		addSounds();
	}

	lastTime_ += frameTime;
	if (lastTime_ < 0.1f) return;

	std::list<LandscapeSoundManagerEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		itor++)
	{
		LandscapeSoundManagerEntry &entry = (*itor);

		// Check if this entry is still relevant
		if (entry.removed) continue;

		// Set this sounds position
		if (!entry.soundType->position->setPosition(
			entry.soundSource, entry.initData))
		{
			// The position set failed, stop this sound and remove it
			entry.soundSource->stop();
			entry.removed = true;
		}
		else
		{
			// Set the volume
			if (entry.soundSource)
			{
				entry.soundSource->setGain(
					(float)OptionsDisplay::instance()->getAmbientSoundVolume() / 128.0f);
			}

			// Check if looped
			if (entry.timeLeft >= 0.0f) 
			{
				// Not looped
				// Check if we play again
				entry.timeLeft -= lastTime_;
				if (entry.timeLeft < 0.0f)
				{
					// Play again
					entry.timeLeft = entry.soundType->timing->getNextEventTime();
					entry.soundType->sound->play(entry.soundSource);
				}
			}
		}
	}

	lastTime_ = 0.0f;
}
