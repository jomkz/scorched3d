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

#include <sprites/ExplosionRingRenderer.h>
#include <common/Defines.h>
#include <sound/SoundUtils.h>
#include <graph/ParticleEmitter.h>
#include <graph/ParticleEngine.h>
#include <client/ScorchedClient.h>

REGISTER_CLASS_SOURCE(ExplosionRingRenderer);


ExplosionRingRenderer::ExplosionRingRenderer()
{
}

ExplosionRingRenderer::~ExplosionRingRenderer()
{
}

void ExplosionRingRenderer::init(unsigned int playerId,
	Vector &position, Vector &velocity, const char *data)
{
	position_ = position;
	data_ = data;
}

void ExplosionRingRenderer::draw(Action *action)
{
}

void ExplosionRingRenderer::simulate(Action *action, float frameTime, bool &remove)
{
	if (0 != strcmp("none", data_.c_str()))
	{
		SoundBuffer *firedSound = 
			Sound::instance()->fetchOrCreateBuffer(
				S3D::getDataFile(data_.c_str()));
		SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
			firedSound, position_);
	}

	// Emmit explosion ring
	ParticleEmitter emmiter;
	emmiter.setAttributes(
		0.6f, 0.6f, // Life
		0.5f, 0.5f, // Mass
		0.0f, 0.0f, // Friction
		Vector(), Vector(), // Velocity
		Vector(0.0f, 0.0f, 0.8f), 0.9f, // StartColor1
		Vector(0.2f, 0.2f, 0.9f), 1.0f, // StartColor2
		Vector(0.6f, 0.6f, 0.95f), 0.0f, // EndColor1
		Vector(0.8f, 0.8f, 1.0f), 0.1f, // EndColor2
		0.2f, 0.2f, 0.5f, 0.5f, // Start Size
		1.5f, 1.5f, 3.0f, 3.0f, // EndSize
		Vector(0.0f, 0.0f, 0.0f), // Gravity
		true,
		false);
	emmiter.emitExplosionRing(
		400, position_, 
		ScorchedClient::instance()->getParticleEngine(), 
		ParticleRendererQuads::getInstance());

	// Remove this action on the first itteration
	remove = true;
}
