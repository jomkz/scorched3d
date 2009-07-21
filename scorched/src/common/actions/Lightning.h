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

#if !defined(__INCLUDE_Lightningh_INCLUDE__)
#define __INCLUDE_Lightningh_INCLUDE__

#include <actions/Action.h>
#include <weapons/WeaponLightning.h>
#include <list>

class GLTexture;
class Target;
class Lightning : public Action
{
public:
	Lightning(WeaponLightning *weapon,
		WeaponFireContext &weaponContext,
		FixedVector &position, FixedVector &velocity);
	virtual ~Lightning();

	virtual void draw();
	virtual void init();
	virtual void simulate(fixed frameTime, bool &remove);
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "Lightning"; }

protected:
	struct Segment
	{
		FixedVector start;
		FixedVector end;
		FixedVector direction;
		bool endsegment;
		fixed size;
	};
	bool firstTime_;
	fixed totalTime_;
	std::list<Segment> segments_;

	GLTexture *texture_;
	WeaponLightning *weapon_;
	FixedVector position_;
	FixedVector velocity_;
	WeaponFireContext weaponContext_;

	void damageTargets(FixedVector &position, 
		std::map<unsigned int, fixed> &hurtMap);
	void dispaceDirection(FixedVector &direction, 
		FixedVector &originalDirection, fixed angle);
	void generateLightning(int id, int depth, fixed size, 
		FixedVector &originalPosition, FixedVector &originalDirection,
		FixedVector &position, FixedVector &direction,
		std::map<unsigned int, fixed> &hurtMap);
};

#endif // __INCLUDE_Lightningh_INCLUDE__
