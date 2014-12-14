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

#if !defined(__INCLUDE_UIStateProgressh_INCLUDE__)
#define __INCLUDE_UIStateProgressh_INCLUDE__

#include <uistate/UIStateI.h>
#include <client/ClientUISync.h>

class UIStateProgress : public UIStateI
{
public:
	UIStateProgress();
	virtual ~UIStateProgress();

	virtual void createState();
	virtual void destroyState();
	virtual void updateState(float frameTime);

	void updateProgress(const LangString &op, float progress);
	void updateDescription(const std::string &name, const std::string &description);
	void updateLandscape();

private:
	Ogre::SceneManager *progressSceneManager_;
	Ogre::Camera *camera_;
	CEGUI::Window *window_, *titleText_, *descriptionText_;
	CEGUI::ProgressBar *progressBar_;

	void createSceneManager();
};

class GUIProgressUpdateThreadCallback : public ThreadCallbackI
{
public:
	GUIProgressUpdateThreadCallback(const LangString &op, const float percentage);
	virtual ~GUIProgressUpdateThreadCallback();

	// ThreadCallbackI
	virtual void callbackInvoked();

private:
	const LangString op_;
	const float percentage_;
};

class GUIProgressDescriptionThreadCallback : public ClientUISyncAction
{
public:
	GUIProgressDescriptionThreadCallback(const std::string &name, const std::string &description);
	virtual ~GUIProgressDescriptionThreadCallback();

	// ClientUISyncAction
	virtual void performUIAction();

private:
	const std::string name_, description_;
};

class GUIProgressLandscapeThreadCallback : public ClientUISyncAction
{
public:
	GUIProgressLandscapeThreadCallback();
	virtual ~GUIProgressLandscapeThreadCallback();

	// ClientUISyncAction
	virtual void performUIAction();
};

#endif // __INCLUDE_UIStateProgressh_INCLUDE__
