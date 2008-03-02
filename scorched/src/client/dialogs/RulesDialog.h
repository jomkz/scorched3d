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

#if !defined(__INCLUDE_RulesDialogh_INCLUDE__)
#define __INCLUDE_RulesDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWButton.h>
#include <GLW/GLWListView.h>
#include <GLW/GLWIcon.h>
#include <GLW/GLWTab.h>

class RulesDialog : public GLWWindow , 
					public GLWButtonI
{
public:
	static RulesDialog *instance();

	virtual void hide();
	virtual void display();
	virtual void buttonDown(unsigned int id);
	virtual void draw();

	void addIcon(GLTexture *texture);

protected:
	static RulesDialog *instance_;
	GLWListView *motdList_, *rulesList_;
	GLWIcon *icon_;
	GLWTab *rulesTab_;
	GLWTab *motdTab_;
	unsigned int okId_;

	void drawRules();
	void addMOTD(const char *text);

private:
	RulesDialog();
	virtual ~RulesDialog();
};

#endif
