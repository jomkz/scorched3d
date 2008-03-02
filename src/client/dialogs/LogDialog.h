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

#if !defined(__INCLUDE_LogDialogh_INCLUDE__)
#define __INCLUDE_LogDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWListView.h>
#include <GLW/GLWTextBox.h>
#include <GLW/GLWLabel.h>
#include <common/LoggerI.h>

class LogDialog : 
	public GLWWindow,
	public GLWButtonI,
	public LoggerI
{
public:
	static LogDialog *instance();

	// Inherited from GLWWindow
	virtual void draw();

	// Inherited from GLWButtonI
	virtual void buttonDown(unsigned int id);

	// LoggerI
	virtual void logMessage(LoggerInfo &info);

	void setServerName(const char *name) { serverName_->setText(name); }

protected:
	static LogDialog *instance_;
	GLWTextButton *quit_;
	GLWLabel *serverName_;
	GLWListView *listView_;

private:
	LogDialog();
	virtual ~LogDialog();
};

#endif
