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

#include <scorched3dc/UIStateProgress.h>

UIStateProgress::UIStateProgress() : UIStateI(UIState::StateProgress)
{
}

UIStateProgress::~UIStateProgress()
{
}

void UIStateProgress::createState()
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

	{
		CEGUI::DefaultWindow* staticText = static_cast<CEGUI::DefaultWindow*>(wmgr.createWindow("OgreTray/StaticText", "StaticText"));
		staticText->setText("");
		// Colours are specified as aarrggbb in Hexadecimal
		// Where aa is alpha, rr is red, gg is green, and bb is blue 
		// tl: top left,  tr: top right,  bl: bottom left,  br: bottom right
		staticText->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
		staticText->setProperty("VertFormatting", "TopAligned"); // TopAligned, BottomAligned, VertCentred
		staticText->setProperty("HorzFormatting", "HorzCentred"); // LeftAligned, RightAligned, HorzCentred
			// HorzJustified, WordWrapLeftAligned, WordWrapRightAligned, WordWrapCentred, WordWrapJustified
		staticText->setTooltipText("This is a StaticText widget");
		staticText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f), CEGUI::UDim(0.15f, 0.0f)));
		staticText->setSize(CEGUI::UVector2(CEGUI::UDim(0.6f, 0.0f), CEGUI::UDim(0.1f, 0.0f)));
		sheet->addChildWindow(staticText);

		CEGUI::ProgressBar* progressBar = static_cast<CEGUI::ProgressBar*>(wmgr.createWindow("OgreTray/ProgressBar", "ProgressBar"));
		progressBar->setProgress(0.0f); // Initial progress of 25%
		progressBar->setStepSize(0.10f); // Calling step() will increase the progress by 10%
		progressBar->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1f, 0.0f), CEGUI::UDim(0.5f, 0.0f)));
		progressBar->setSize(CEGUI::UVector2(CEGUI::UDim(0.8f, 0.0f), CEGUI::UDim(0.3f, 0.0f)));
		staticText->addChildWindow(progressBar);
	}

	CEGUI::System::getSingleton().setGUISheet(sheet);
}

void UIStateProgress::destroyState()
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.destroyAllWindows();
}
