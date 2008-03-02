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

#if !defined(__INCLUDE_GLWChannelViewh_INCLUDE__)
#define __INCLUDE_GLWChannelViewh_INCLUDE__

#include <GLW/GLWidget.h>
#include <GLW/GLWIconButton.h>
#include <GLW/GLWChannelViewTextRenderer.h>
#include <client/ClientChannelManagerI.h>
#include <common/KeyboardKey.h>
#include <common/Vector.h>
#include <list>

class GLWChannelViewI
{
public:
	virtual ~GLWChannelViewI();

	virtual void channelsChanged(unsigned int id) = 0;
};

class GLWChannelView : 
	public GLWidget,
	public ClientChannelManagerI,
	public GLWButtonI
{
public:
	class BaseChannelEntry
	{
	public:
		std::string channel;
		unsigned int type;
	};
	class CurrentChannelEntry : public BaseChannelEntry
	{
	public:
		Vector color;
		unsigned int id;
	};

	GLWChannelView();
	virtual ~GLWChannelView();

	bool getParentSized() { return parentSized_; }
	bool initFromXMLInternal(XMLNode *node);

	unsigned int getLastWhisperSrc() { return lastWhisperSrc_; }

	std::list<CurrentChannelEntry> &getCurrentChannels() { return currentChannels_; }
	std::list<BaseChannelEntry> &getAvailableChannels() { return availableChannels_; }
	CurrentChannelEntry *getChannel(const char *channelName);

	void joinChannel(const char *channelName);
	void leaveChannel(const char *channelName);

	void setHandler(GLWChannelViewI *handler) { handler_ = handler; };

	// GLWidget
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual bool initFromXML(XMLNode *node);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);
	virtual void setX(float x);
	virtual void setY(float y);

	// ClientChannelManagerI
	virtual void channelText(ChannelText &text);
	virtual void registeredForChannels(
		std::list<ChannelDefinition> &registeredChannels,
		std::list<ChannelDefinition> &availableChannels);

	// ButtonI
	virtual void buttonDown(unsigned int id);

	REGISTER_CLASS_HEADER(GLWChannelView);

protected:
	class GLWChannelViewEntry
	{
	public:
		Vector color;
		GLWChannelViewTextRenderer text;
		float timeRemaining;
	};

	GLWChannelViewI *handler_;
	GLWIconButton upButton_;
	GLWIconButton downButton_;
	GLWIconButton resetButton_;
	GLTexture upTexture_;
	GLTexture downTexture_;
	GLTexture resetTexture_;

	unsigned int lastChannelId_;
	unsigned int lastWhisperSrc_;
	bool init_;
	bool alignTop_, parentSized_;
	bool splitLargeLines_, allowScroll_;
	bool showChannelName_, showChannelNumber_;
	int lineDepth_;
	int scrollPosition_;
	float displayTime_;
	float fontSize_, outlineFontSize_;
	int visibleLines_, totalLines_;
	int currentVisible_;
	std::list<GLWChannelViewEntry> textLines_;
	KeyboardKey *scrollUpKey_;
	KeyboardKey *scrollDownKey_;
	KeyboardKey *scrollResetKey_;
	std::map<std::string, Vector> channelColors_;
	std::list<std::string> startupChannels_;
	std::list<CurrentChannelEntry> currentChannels_;
	std::list<BaseChannelEntry> availableChannels_;
	std::string textSound_;

	void addInfo(Vector &color, GLWChannelViewTextRenderer &text);
	void formCurrentChannelList(std::list<std::string> &result);
	int splitLine(const char *message);
};

#endif // __INCLUDE_GLWChannelViewh_INCLUDE__
