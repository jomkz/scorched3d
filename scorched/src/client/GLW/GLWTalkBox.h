////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_GLWTalkBoxh_INCLUDE__)
#define __INCLUDE_GLWTalkBoxh_INCLUDE__

#include <GLW/GLWidget.h>
#include <lang/LangString.h>

class GLWTalkBox : public GLWidget
{
public:
	enum TalkMode
	{
		eSay,
		eTeamSay
	};

	GLWTalkBox();
	virtual ~GLWTalkBox();

	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);
	virtual void display();

	virtual bool initFromXML(XMLNode *node);

	REGISTER_CLASS_HEADER(GLWTalkBox);
protected:
	LangString text_;
	float ctime_;
	bool cursor_;
	int maxTextLen_;
	bool parentSized_;
	TalkMode mode_;
	TalkMode defaultMode_;
};

#endif // __INCLUDE_GLWTalkBoxh_INCLUDE__
