////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#if !defined(__INCLUDE_GUIProgressCounterh_INCLUDE__)
#define __INCLUDE_GUIProgressCounterh_INCLUDE__

#include <common/ProgressCounter.h>
#include <engine/ThreadCallbackI.h>
#include <engine/ThreadCallback.h>

class GUIProgress
{
public:
	static GUIProgress *instance();

	void setVisible(bool visible);
	bool isVisible(); 

	CEGUI::Window *getStaticText() { return staticText_; }
	CEGUI::ProgressBar *getProgressBar() { return progressBar_; }

protected:
	void create();   
 
	float totalTime_;
	int frameCount_;
	CEGUI::Window *window_, *staticText_;
	CEGUI::ProgressBar *progressBar_;
private:
	GUIProgress();
	virtual ~GUIProgress();

};

class GUIProgressThreadCallback : public ThreadCallbackI
{
public:
	GUIProgressThreadCallback(const LangString &op, const float percentage);
	virtual ~GUIProgressThreadCallback();

	// ThreadCallbackI
	virtual void callbackInvoked();

private:
	const LangString op_;
	const float percentage_;
};

class GUIProgressCounter : public ProgressCounterI
{
public:
	static ProgressCounter *instance();

	void updateProgress();

	// ProgressCounterI
	// ** Called from the client and server so careful with threading **
	virtual void operationChange(const LangString &op);
	virtual void progressChange(const LangString &op, const float percentage);

protected:
	time_t lastTime_;
	// A seperate callback mechanism so we can get progress when
	// the client is blocked and cant use uisync
	// Most other things should use this as it may cause
	// issues with sequencing of other actions being performed via uisync
	ThreadCallback threadCallback_;

private:
	static ProgressCounter *instance_;

	GUIProgressCounter ();
	virtual ~GUIProgressCounter ();	
};

#endif
