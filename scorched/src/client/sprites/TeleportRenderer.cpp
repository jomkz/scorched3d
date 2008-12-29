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

#include <sprites/TeleportRenderer.h>
#include <graph/ParticleEmitter.h>
#include <client/ScorchedClient.h>
#include <sprites/ExplosionTextures.h>

TeleportRenderer::TeleportRenderer(Vector &position, Vector &color) : 
	position_(position), color_(color)
{
}

TeleportRenderer::~TeleportRenderer()
{
}

void TeleportRenderer::draw(Action *action)
{
}

void TeleportRenderer::simulate(Action *action, float timepassed, bool &remove)
{
	remove = true;
	ParticleEmitter emitter;
	emitter.setAttributes(
		2.5f, 2.0f, // Life
		0.2f, 0.5f, // Mass
		0.01f, 0.02f, // Friction
		Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
		color_, 1.0f, // StartColor1
		color_, 1.0f, // StartColor2
		color_, 0.0f, // EndColor1
		color_, 0.0f, // EndColor2
		0.7f, 1.0f, 0.7f, 1.0f, // Start Size
		0.7f, 1.0f, 0.7f, 1.0f, // EndSize
		Vector(0.0f, 0.0f, 0.0f), // Gravity
		false,
		false);

	GLTextureSet *set = ExplosionTextures::instance()->getTextureSetByName("trans");
	if (!set) return;

	emitter.emitTransport(position_,
		ScorchedClient::instance()->getParticleEngine(),
		set);
}
