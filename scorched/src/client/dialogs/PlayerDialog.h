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

#if !defined(__INCLUDE_PlayerDialogh_INCLUDE__)
#define __INCLUDE_PlayerDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWButton.h>
#include <GLW/GLWTextBox.h>
#include <GLW/GLWImageList.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWDropDownColor.h>
#include <GLEXT/GLTexture.h>
#include <GLW/GLWTankViewer.h>

class PlayerDialog : public GLWWindow,
	public GLWButtonI, public GLWDropDownI
{
public:
	static PlayerDialog *instance();

	virtual void display();
	virtual void draw();

	// Inherited from GLWWindow
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);

	// Inherited from GLWButtonI
	virtual void buttonDown(unsigned int id);

	// GLWDropDownI
	virtual void select(unsigned int id, const int pos, 
		GLWSelectorEntry value);

protected:
	PlayerDialog();
	virtual ~PlayerDialog();

	static PlayerDialog *instance_;
	GLWDropDownText *typeDropDown_;
	GLWDropDownText *teamDropDown_;
	GLWDropDownColor *colorDropDown_;
	GLWLabel *colorLabel_;
	GLWLabel *teamLabel_;
	GLWTankViewer *viewer_;
	GLWTextBox *playerName_;
	GLWImageList *imageList_;
	ToolTip avatarTip1_, avatarTip2_;
	ToolTip humanToolTip_;
	GLTexture colorTexture_;
	unsigned int allocatedTeam_;
	unsigned int okId_, cancelId_;
	unsigned int currentPlayerId_;

	void nextPlayer();
	int getCurrentTeam();
	unsigned int getNextPlayer(unsigned int current);

};

#endif
