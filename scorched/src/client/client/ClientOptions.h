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

#include <XML/XMLEntrySimpleTypes.h>
#include <console/ConsoleRuleOptionsAdapter.h>

class ClientOptions : public XMLEntrySimpleGroup
{
public:
	static ClientOptions *instance();

	bool getWaterAnimate() { return waterAnimate_.getValue(); }
	XMLEntryBool &getWaterAnimateEntry() { return waterAnimate_; }

	bool getWaterDraw() { return waterDraw_.getValue(); }
	XMLEntryBool &getWaterDrawEntry() { return waterDraw_; }

	bool getWaterWireframe() { return waterWireframe_.getValue(); }
	XMLEntryBool &getWaterWireframeEntry() { return waterWireframe_; }

	bool getSkyDraw() { return skyDraw_.getValue(); }
	XMLEntryBool &getSkyDrawEntry() { return skyDraw_; }

	bool getLandscapeDraw() { return landscapeDraw_.getValue(); }
	XMLEntryBool &getLandscapeDrawEntry() { return landscapeDraw_; }

	bool getLandscapeGridDraw() { return landscapeGridDraw_.getValue(); }
	XMLEntryBool &getLandscapeGridDrawEntry() { return landscapeGridDraw_; }

	bool getCameraWireframe() { return cameraWireframe_.getValue(); }
	XMLEntryBool &getCameraWireframeEntry() { return cameraWireframe_; }

	bool getTargetsDraw() { return targetsDraw_.getValue(); }
	XMLEntryBool &getTargetsDrawEntry() { return targetsDraw_; }

	bool getShadows() { return shadows_.getValue(); }
	XMLEntryBool &getShadowsEntry() { return shadows_; }

	bool getValidateServerIp() { return validateServerIp_.getValue(); }
	XMLEntryBool &getValidateServerIpEntry() { return validateServerIp_; }

	const char *getHostDescription() { return hostDescription_.getValue().c_str(); }
	XMLEntryString &getHostDescriptionEntry() { return hostDescription_; }

	void loadSafeValues();
	void loadDefaultValues();
	void loadMediumValues();
	void loadFastestValues();
	void loadUltraValues();
	bool writeOptionsToFile();
	bool readOptionsFromFile();

	void addToConsole(Console &console) { consoleHolder_.addToConsole(console, xmlEntryChildren_); }

protected:
	static ClientOptions *instance_;

	ConsoleRuleOptionsAdapterHolder consoleHolder_;
	XMLEntryBool waterAnimate_;
	XMLEntryBool waterDraw_;
	XMLEntryBool waterWireframe_;
	XMLEntryBool skyDraw_;
	XMLEntryBool landscapeDraw_, landscapeGridDraw_;
	XMLEntryBool cameraWireframe_;
	XMLEntryBool targetsDraw_;
	XMLEntryBool shadows_;
	XMLEntryBool validateServerIp_;
	XMLEntryString hostDescription_;

private:
	ClientOptions();
	virtual ~ClientOptions();

};

#endif
