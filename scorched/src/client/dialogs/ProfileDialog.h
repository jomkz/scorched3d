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

#if !defined(__INCLUDE_ProfileDialogh_INCLUDE__)
#define __INCLUDE_ProfileDialogh_INCLUDE__

#include <GLW/GLWWindow.h>

class Tank;
class ProfileDialog : public GLWWindow 
{
public:
	static ProfileDialog *instance();

	// Inherited from GLWWindow
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void saveSettings(XMLNode *node);
	virtual void loadSettings(XMLNode *node, bool resetPositions);

protected:
	static ProfileDialog *instance_;
	float profileZoom_, zoomX_, zoomY_;
	bool zooming_;
	float ox_, oy_, ow_, oh_;

	void drawLandscape(Tank *currentTank);
	void drawTanks(Tank *currentTank);
	void drawAiming(Tank *currentTank);

private:
	ProfileDialog();
	virtual ~ProfileDialog();
};

#endif

