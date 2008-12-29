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

#if !defined(__INCLUDE_ViewPointsh_INCLUDE__)
#define __INCLUDE_ViewPointsh_INCLUDE__

#include <list>
#include <common/FixedVector.h>

class ScorchedContext;
class ViewPoints
{
public:
	class ViewPoint 
	{
	public:
		ViewPoint() : radius_(0), from_(1, 0, 0) { }

		void setLookFrom(FixedVector &from) { from_ = from; }
		FixedVector &getLookFrom() { return from_; }
		
		void setPosition(FixedVector &pos) { position_ = pos; }
		FixedVector &getPosition() { return position_; }

		void setRadius(fixed radius) { radius_ = radius; }
		fixed getRadius() { return radius_; }

	protected:
		fixed radius_;
		FixedVector position_;
		FixedVector from_;
	};

	ViewPoints();
	virtual ~ViewPoints();

	void reset() { finished_ = false; }
	void explosion(unsigned int playerId);

	void simulate(fixed frameTime);
	void getValues(FixedVector &lookAt, 
				   FixedVector &lookFrom);
	void setValues(FixedVector &lookAt, 
				   FixedVector &lookFrom);
	int getLookAtCount();

	ViewPoints::ViewPoint *getNewViewPoint(unsigned int playerId);
	void releaseViewPoint(ViewPoints::ViewPoint *point);

	void setContext(ScorchedContext *context) { context_ = context; }

protected:
	std::list<ViewPoint *> points_;
	ScorchedContext *context_;
	FixedVector lookAt_, lookFrom_;
	fixed totalTime_;
	bool finished_;

};

#endif
