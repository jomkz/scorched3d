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

#if !defined(__INCLUDE_ClientOptionsh_INCLUDE__)
#define __INCLUDE_ClientOptionsh_INCLUDE__

#include <common/OptionEntry.h>
#include <console/ConsoleRuleOptionsAdapter.h>

class ClientOptions
{
public:
	static ClientOptions *instance();

	bool getWaterAnimate() { return waterAnimate_; }
	OptionEntryBool &getWaterAnimateEntry() { return waterAnimate_; }

	bool getWaterDraw() { return waterDraw_; }
	OptionEntryBool &getWaterDrawEntry() { return waterDraw_; }

	bool getWaterWireframe() { return waterWireframe_; }
	OptionEntryBool &getWaterWireframeEntry() { return waterWireframe_; }

	bool getSkyDraw() { return skyDraw_; }
	OptionEntryBool &getSkyDrawEntry() { return skyDraw_; }

	bool getLandscapeDraw() { return landscapeDraw_; }
	OptionEntryBool &getLandscapeDrawEntry() { return landscapeDraw_; }

	bool getLandscapeGridDraw() { return landscapeGridDraw_; }
	OptionEntryBool &getLandscapeGridDrawEntry() { return landscapeGridDraw_; }

	bool getCameraWireframe() { return cameraWireframe_; }
	OptionEntryBool &getCameraWireframeEntry() { return cameraWireframe_; }

	bool getTargetsDraw() { return targetsDraw_; }
	OptionEntryBool &getTargetsDrawEntry() { return targetsDraw_; }

	bool getShadows() { return shadows_; }
	OptionEntryBool &getShadowsEntry() { return shadows_; }

	bool getValidateServerIp() { return validateServerIp_; }
	OptionEntryBool &getValidateServerIpEntry() { return validateServerIp_; }

	const char *getHostDescription() { return hostDescription_; }
	OptionEntryString &getHostDescriptionEntry() { return hostDescription_; }

	void loadSafeValues();
	void loadDefaultValues();
	void loadMediumValues();
	void loadFastestValues();
	void loadUltraValues();
	bool writeOptionsToFile(bool allOptions);
	bool readOptionsFromFile();

	std::list<OptionEntry *> &getOptions() { return options_; }
	void addToConsole(Console &console) { consoleHolder_.addToConsole(console, options_); }

protected:
	static ClientOptions *instance_;
	std::list<OptionEntry *> options_;

	ConsoleRuleOptionsAdapterHolder consoleHolder_;
	OptionEntryBool waterAnimate_;
	OptionEntryBool waterDraw_;
	OptionEntryBool waterWireframe_;
	OptionEntryBool skyDraw_;
	OptionEntryBool landscapeDraw_, landscapeGridDraw_;
	OptionEntryBool cameraWireframe_;
	OptionEntryBool targetsDraw_;
	OptionEntryBool shadows_;
	OptionEntryBool validateServerIp_;
	OptionEntryString hostDescription_;

private:
	ClientOptions();
	virtual ~ClientOptions();

};

#endif
