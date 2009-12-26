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

#if !defined(__INCLUDE_Particleh_INCLUDE__)
#define __INCLUDE_Particleh_INCLUDE__

#include <common/Vector.h>
#include <graph/ParticleRenderer.h>
#include <GLEXT/GLTextureSet.h>

class ParticleUserData
{
public:
	virtual ~ParticleUserData() {}
};

class ParticleEngine;
class Particle
{
public:
	Particle();
	virtual ~Particle();

	void setParticle(
		float life, float mass, float friction,
		Vector &velocity, Vector &gravity,
		Vector &color, Vector &colorCounter,
		Vector &size, Vector &sizeCounter,
		float alpha, float alphaCounter,
		bool additiveTexture,
		bool windAffect);
	void unsetParticle();

	float life_;
	float mass_;
	float friction_;
	float percent_, percentCounter_;
	Vector position_;
	Vector velocity_;
	Vector gravity_;
	Vector color_, colorCounter_;
	Vector size_, sizeCounter_;
	float alpha_, alphaCounter_;
	bool windAffect_;
	ParticleEngine *engine_;
	unsigned int type_;

	// Used for texturing
	bool additiveTexture_;
	GLTexture *texture_;
	GLTextureSet *textureSet_;
	int textureCoord_;
	bool shadow_;
	bool simulated_;

	float distance_; // Dist from camera
	ParticleUserData *userData_;
	ParticleRenderer *renderer_; // How to render
};

#endif // __INCLUDE_Particleh_INCLUDE__
