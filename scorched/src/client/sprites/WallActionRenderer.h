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

#if !defined(AFX_SCENEWALLACTION_H__8D1048A1_FFC6_4950_9C56_8163F8FB2F07__INCLUDED_)
#define AFX_SCENEWALLACTION_H__8D1048A1_FFC6_4950_9C56_8163F8FB2F07__INCLUDED_

#include <engine/Action.h>
#include <common/Vector.h>
#include <common/OptionsTransient.h>
#include <graph/Particle.h>
#include <GLEXT/GLTexture.h>

class WallActionRenderer : public ParticleUserData
{
public:
	WallActionRenderer(Vector &position, OptionsTransient::WallSide type);
	virtual ~WallActionRenderer();

	void simulate(float frameTime);
	void draw();

protected:
	static GLTexture texture_;
	Vector position_;
	OptionsTransient::WallSide type_;
	bool init_;
	float fade_;
	float xOff_, yOff_;
	Vector color_;
	Vector cornerA_, cornerB_;
	Vector cornerC_, cornerD_;

	void init();

};

#endif // !defined(AFX_SCENEWALLACTION_H__8D1048A1_FFC6_4950_9C56_8163F8FB2F07__INCLUDED_)
