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

#if !defined(AFX_GLWSpacer_H__75483479_A6F8_45CC_8E83_B517E721211F__INCLUDED_)
#define AFX_GLWSpacer_H__75483479_A6F8_45CC_8E83_B517E721211F__INCLUDED_

#include <GLW/GLWidget.h>

class GLWSpacer : public GLWidget  
{
public:
	GLWSpacer(float w = 0.0f, float h = 0.0f);
	virtual ~GLWSpacer();

	REGISTER_CLASS_HEADER(GLWSpacer);
};

#endif // !defined(AFX_GLWSpacer_H__75483479_A6F8_45CC_8E83_B517E721211F__INCLUDED_)
