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

#if !defined(__INCLUDE_MsgBoxDialogh_INCLUDE__)
#define __INCLUDE_MsgBoxDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWIcon.h>

class MsgBoxDialog : public GLWWindow,
	public GLWButtonI
{
public:
	static MsgBoxDialog *instance();

	void show(const std::string &message);

	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	virtual void buttonDown(unsigned int id);

protected:
	static MsgBoxDialog *instance_;

	GLWTextButton *okButton_;
	GLWIcon *icon_;
	GLWLabel *message_;

private:
	MsgBoxDialog();
	virtual ~MsgBoxDialog();

};

#endif

