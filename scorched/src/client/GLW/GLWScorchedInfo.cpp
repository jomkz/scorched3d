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

#include <GLW/GLWScorchedInfo.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWWindView.h>
#include <client/ScorchedClient.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankAvatar.h>
#include <tank/TankScore.h>
#include <tank/TankAccessories.h>
#include <tank/TankPosition.h>
#include <target/TargetParachute.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <weapons/AccessoryStore.h>

REGISTER_CLASS_SOURCE(GLWHudCondition);

GLWHudCondition::GLWHudCondition()
{
}

GLWHudCondition::~GLWHudCondition()
{
}

bool GLWHudCondition::getResult(GLWidget *widget)
{
	return TargetRendererImplTankHUD::drawText();
}

REGISTER_CLASS_SOURCE(GLWScorchedInfo);

GLWScorchedInfo::GLWScorchedInfo(float x, float y, float w, float h) : 
	GLWidget(x, y, w, h), infoType_(eNone), noCenter_(false)
{

}

GLWScorchedInfo::~GLWScorchedInfo()
{

}

void GLWScorchedInfo::draw()
{
	Vector *fontColor = &fontColor_;
	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();
	if (inBox((float) mouseX - GLWTranslate::getPosX(), 
		(float) mouseY - GLWTranslate::getPosY(), x_, y_, w_, h_))
	{
		fontColor = &selectedColor_;
	}

	GLWidget::draw();
	
	GLState state(GLState::TEXTURE_ON | GLState::DEPTH_OFF);
	
	// Items not relating to the current player
	switch(infoType_)
	{
		case eWind:
		{
			static WindDialogToolTip windTip;
			setToolTip(&windTip);
			static char buffer[256];
			if (ScorchedClient::instance()->
				getOptionsTransient().getWindSpeed() == 0)
			{
				snprintf(buffer, 256, "No Wind");
			}
			else
			{
				snprintf(buffer, 256, "Force %.0f", 
					ScorchedClient::instance()->
					getOptionsTransient().getWindSpeed().asFloat());
			}
			float windwidth = GLWFont::instance()->
				getGameFont()->getWidth(
				fontSize_, buffer);
			float offSet = 0.0f;
			if (!noCenter_) offSet = w_ / 2.0f - (windwidth / 2.0f);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_ + offSet, y_, 0.0f,
				buffer);                    
		}
		break;
	}
	
	// Get the current tank and model
	Tank *current = 
 		ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (!current ||
		current->getState().getState() != TankState::sNormal)
  	{
  		return;
	}
	TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
		current->getRenderer();
	if (!renderer) return;

	// Items relating to the current player
	switch (infoType_)
	{
		case ePlayerName:
		{
			setToolTip(&renderer->getTips()->nameTip);
			float namewidth = GLWFont::instance()->getGameFont()->getWidth(
				fontSize_, current->getName());
			float offSet = 0.0f;
			if (!noCenter_) offSet = w_ / 2.0f - (namewidth / 2.0f);
			GLWFont::instance()->getGameFont()->draw(
				current->getColor(), fontSize_,
				x_ + offSet, y_, 0.0f,
				current->getName());
		}
		break;
		case ePlayerIcon:
		{
			setToolTip(&renderer->getTips()->nameTip);
			GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON);
			glColor3f((*fontColor)[0], (*fontColor)[1], (*fontColor)[2]);
			current->getAvatar().getTexture()->draw();
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(x_, y_);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(x_ + w_, y_);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(x_ + w_, y_ + h_);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(x_, y_ + h_);
			glEnd();
		}
		break;
		case ePlayerColor:
		{
			setToolTip(&renderer->getTips()->nameTip);
			GLWFont::instance()->getGameFont()->draw(
				current->getColor(), fontSize_,
				x_, y_, 0.0f,
				current->getScore().getStatsRank());
		}
		break;
		case eAutoDefenseCount:
			setToolTip(&renderer->getTips()->autodTip);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				(current->getAccessories().getAutoDefense().haveDefense()?"On":"Off"));
		break;
		case eParachuteCount:
			setToolTip(&renderer->getTips()->paraTip);
			if (!current->getParachute().getCurrentParachute())
			{
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					"Off");
			}
			else
			{
				int count = current->getAccessories().getAccessoryCount(
					current->getParachute().getCurrentParachute());
				char buffer[128];
				if (count >= 0) snprintf(buffer, 128, "%i", count);
				else snprintf(buffer, 128, "In");
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					S3D::formatStringBuffer("%s", buffer));
			}
		break;
		case eHealthCount:
			setToolTip(&renderer->getTips()->healthTip);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer("%.0f", current->getLife().getLife().asFloat()));
		break;
		case eShieldCount:
			setToolTip(&renderer->getTips()->shieldTip);
			if (!current->getShield().getCurrentShield())
			{
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					"Off");
			}
			else
			{
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					S3D::formatStringBuffer("%.0f", current->getShield().getShieldPower().asFloat()));
			}
		break;
		case eBatteryCount:
			{
			int count = current->getAccessories().getBatteries().getNoBatteries();
			setToolTip(&renderer->getTips()->batteryTip);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer((count==-1?"In":"%i"),	count));
			}
		break;
		case eFuelCount:
			{
			Accessory *weapon = current->getAccessories().getWeapons().getCurrent();

			setToolTip(&renderer->getTips()->fuelTip);

			if (!weapon ||
				weapon->getPositionSelect() != Accessory::ePositionSelectFuel)
			{
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					"Off");
			}
			else
			{
				int count = current->getAccessories().getAccessoryCount(weapon);
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					S3D::formatStringBuffer((count==-1?"In":"%i"), count));
			}
			}
		break;
		case eWeaponName:
		{
			Accessory *weapon = current->getAccessories().getWeapons().getCurrent();
			if (!weapon) return;

			setToolTip(&renderer->getTips()->weaponTip);

			static char buffer[256];
			snprintf(buffer, 256, "%s", weapon->getName());
			float weaponWidth = (float) GLWFont::instance()->getGameFont()->
				getWidth(fontSize_, buffer);

			float offSet = 0.0f;
			if (!noCenter_) offSet = w_ / 2.0f - (weaponWidth / 2.0f);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_ + offSet, y_, 0.0f,
				buffer);
		}
		break;
		case eWeaponCount:
		{
			Accessory *weapon = current->getAccessories().getWeapons().getCurrent();
			if (!weapon) return;

			setToolTip(&renderer->getTips()->weaponTip);
			int count = current->getAccessories().getAccessoryCount(weapon);
			const char *format = "%i";
			if (count < 0) format = "In";
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer(format, count));
		}
		break;
		case eWeaponIcon:
		{
			Accessory *weapon = current->getAccessories().getWeapons().getCurrent();
			if (!weapon) return;

			setToolTip(&renderer->getTips()->weaponTip);

			GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON);
			glColor3f((*fontColor)[0], (*fontColor)[1], (*fontColor)[2]);
			weapon->getTexture()->draw();
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(x_, y_);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(x_ + w_, y_);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(x_ + w_, y_ + h_);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(x_, y_ + h_);
			glEnd();
		}
		break;
		case eRotation:
			setToolTip(&renderer->getTips()->rotationTip);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer("%.1f",
				360.0f - current->getPosition().getRotationGunXY().asFloat()));
		break;
		case eRotationDiff:
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer("%+.1f",
				current->getPosition().getRotationXYDiff().asFloat()));
		break;
		case eElevation:
			setToolTip(&renderer->getTips()->elevationTip);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer("%.1f",
				current->getPosition().getRotationGunYZ().asFloat()));
		break;
		case eElevationDiff:
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer("%+.1f",
				current->getPosition().getRotationYZDiff().asFloat()));
		break;
		case ePower:
			setToolTip(&renderer->getTips()->powerTip);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer("%.1f",
				current->getPosition().getPower().asFloat()));
		break;
		case ePowerDiff:
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				S3D::formatStringBuffer("%+.1f",
				current->getPosition().getPowerDiff().asFloat()));
		break;
	}
}

void GLWScorchedInfo::mouseDown(int button, float x, float y, bool &skipRest)
{
	Tank *current = 
 		ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (!current ||
		current->getState().getState() != TankState::sNormal)
  	{
  		return;
	}
	TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
		current->getRenderer();
	if (!renderer) return;
	GLWTankTips *tankTips = renderer->getTips();

	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		switch (infoType_)
		{
			case ePlayerName:
			break;
			case ePlayerIcon:
			break;
			case ePlayerColor:
			break;
			case eAutoDefenseCount:
				tankTips->autodTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eParachuteCount:
				tankTips->paraTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eHealthCount:
			break;
			case eShieldCount:
				tankTips->shieldTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eBatteryCount:
				tankTips->batteryTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eFuelCount:
				tankTips->fuelTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eWeaponName:
				tankTips->weaponTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eWeaponCount:
				tankTips->weaponTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eWeaponIcon:
				tankTips->weaponTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eRotation:
				tankTips->undoMenu.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eElevation:
				tankTips->undoMenu.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case ePower:
				tankTips->undoMenu.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
		}
	}
	else
	{
		GLWidget::mouseDown(button, x, y, skipRest);
	}
}

bool GLWScorchedInfo::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;

	// Type node
	XMLNode *typeNode = 0;
	if (!node->getNamedChild("type", typeNode)) return false;
	if (0 == strcmp(typeNode->getContent(), "wind")) infoType_ = eWind;
	else if (0 == strcmp(typeNode->getContent(), "playername")) infoType_ = ePlayerName;
	else if (0 == strcmp(typeNode->getContent(), "playericon")) infoType_ = ePlayerIcon;
	else if (0 == strcmp(typeNode->getContent(), "playercolor")) infoType_ = ePlayerColor;
	else if (0 == strcmp(typeNode->getContent(), "autodefensecount")) infoType_ = eAutoDefenseCount;
	else if (0 == strcmp(typeNode->getContent(), "parachutecount")) infoType_ = eParachuteCount;
	else if (0 == strcmp(typeNode->getContent(), "shieldcount")) infoType_ = eShieldCount;
	else if (0 == strcmp(typeNode->getContent(), "healthcount")) infoType_ = eHealthCount;
	else if (0 == strcmp(typeNode->getContent(), "fuelcount")) infoType_ = eFuelCount;
	else if (0 == strcmp(typeNode->getContent(), "batterycount")) infoType_ = eBatteryCount;
	else if (0 == strcmp(typeNode->getContent(), "weaponname")) infoType_ = eWeaponName;
	else if (0 == strcmp(typeNode->getContent(), "weaponcount")) infoType_ = eWeaponCount;
	else if (0 == strcmp(typeNode->getContent(), "weaponicon")) infoType_ = eWeaponIcon;
	else if (0 == strcmp(typeNode->getContent(), "rotation")) infoType_ = eRotation;
	else if (0 == strcmp(typeNode->getContent(), "elevation")) infoType_ = eElevation;
	else if (0 == strcmp(typeNode->getContent(), "power")) infoType_ = ePower;
	else if (0 == strcmp(typeNode->getContent(), "rotationdiff")) infoType_ = eRotationDiff;
	else if (0 == strcmp(typeNode->getContent(), "elevationdiff")) infoType_ = eElevationDiff;
	else if (0 == strcmp(typeNode->getContent(), "powerdiff")) infoType_ = ePowerDiff;
	else
	{
		S3D::dialogMessage("GLWScorchedInfo", S3D::formatStringBuffer(
			"Unknown info type \"%s\"",
			typeNode->getContent()));
		return false;
	}
	
	// Font Size
	if (!node->getNamedChild("fontsize", fontSize_)) return false;
	
	// Font Color
	if (!node->getNamedChild("fontcolorr", fontColor_[0])) return false;
	if (!node->getNamedChild("fontcolorg", fontColor_[1])) return false;
	if (!node->getNamedChild("fontcolorb", fontColor_[2])) return false;

	// Font Selected Size
	if (!node->getNamedChild("selfontcolorr", selectedColor_[0], false))  selectedColor_[0] = fontColor_[0];
	if (!node->getNamedChild("selfontcolorg", selectedColor_[1], false))  selectedColor_[1] = fontColor_[1];
	if (!node->getNamedChild("selfontcolorb", selectedColor_[2], false))  selectedColor_[2] = fontColor_[2];

	// No Center
	XMLNode *centerNode;
	if (node->getNamedChild("nocenter", centerNode, false))
	{
		noCenter_ = (0 == strcmp(centerNode->getContent(), "true"));
	}

	return true;
}
