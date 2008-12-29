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

#if !defined(AFX_TankCamera_H__83501862_9536_4108_A7E6_2377AD98EB72__INCLUDED_)
#define AFX_TankCamera_H__83501862_9536_4108_A7E6_2377AD98EB72__INCLUDED_

#include <common/Vector.h>

class ScorchedContext;
class TankCamera  
{
public:
	TankCamera(ScorchedContext &context);
	virtual ~TankCamera();

	int getCameraType() { return cameraType_; }
	void setCameraType(int c) { cameraType_ = c; }

	Vector &getCameraRotation() { return cameraRotation_; }
	void setCameraRotation(Vector &rot) { cameraRotation_ = rot; }
	
	Vector &getCameraLookAt() { return lookAt_; }
	void setCameraLookAt(Vector &pos) { lookAt_ = pos; }

protected:
	ScorchedContext &context_;
	Vector cameraRotation_, lookAt_;
	int cameraType_;
};

#endif // !defined(AFX_TANKBBATTERIES_H__83501862_9536_4108_A7E6_2377AD98EB72__INCLUDED_)
