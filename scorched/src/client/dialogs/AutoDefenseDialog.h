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

#if !defined(__INCLUDE_AutoDefenseDialogh_INCLUDE__)
#define __INCLUDE_AutoDefenseDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWButton.h>
#include <GLW/GLWDropDownText.h>
#include <dialogs/BuyAccessoryDialogTankInfo.h>

class AutoDefenseDialog :
	public GLWWindow, 
	public GLWButtonI,
	public GLWDropDownI
{
public:
	static AutoDefenseDialog *instance();

	// Inherited from GLWButtonI
	virtual void buttonDown(unsigned int id);

	// Inherited from GLWDropDownI 
	virtual void select(unsigned int id, const int pos, GLWSelectorEntry value);

	// Inherited from GLWWindow
	virtual void windowInit(const unsigned state);

protected:
	BuyAccessoryDialogTankInfo &tankInfo_;
	unsigned int okId_;
	unsigned int cancelId_;
	GLWPanel *topPanel_;
	GLWDropDownText *ddshields_;
	GLWDropDownText *ddpara_;

	void finished();
	void displayCurrent();

private:
	AutoDefenseDialog();
	virtual ~AutoDefenseDialog();
};


#endif
