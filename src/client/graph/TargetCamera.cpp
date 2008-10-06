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

#include <graph/TargetCamera.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <actions/CameraPositionAction.h>
#include <weapons/AccessoryStore.h>
#include <weapons/WeaponMoveTank.h>
#include <graph/OptionsDisplay.h>
#include <landscapemap/MovementMap.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <water/Water.h>
#include <engine/ViewPoints.h>
#include <engine/GameState.h>
#include <tankgraph/TankKeyboardControlUtil.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <tank/TankAccessories.h>
#include <target/TargetLife.h>
#include <common/ChannelManager.h>
#include <common/Keyboard.h>
#include <common/Logger.h>
#include <common/LoggerI.h>
#include <common/Defines.h>
#include <lang/LangResource.h>
#include <math.h>

static const char *cameraNames[] = 
{
	"Top",
	"AboveTank",
	"Tank",
	"Shot",
	"Action",
	"Left",
	"Right",
	"LeftFar",
	"RightFar",
	"Spectator",
	"Free"
};
static const int noCameraNames = sizeof(cameraNames) / sizeof(char *);

static ToolTip *cameraToolTips = 0;
static const char *cameraDescriptions[] = 
{
	"Look directly down on the current tank.\n"
	"Tracks the current tanks rotation.",
	"Look from above and behind the current tank.\n"
	"Tracks the current tanks rotation.",
	"Look from directly behind the current tank.\n"
	"Tracks the current tanks rotation.",
	"Look from the current tanks gun turret.\n"
	"Follows any shots the current tank makes.\n"
	"Tracks the current tanks rotation.",
	"Automaticaly tracks any action around the\n"
	"island by moving to view any explosions\n"
	"and deaths.",
	"Look at the left of the current tank.\n"
	"Tracks the current tanks rotation.",
	"Look at the right of the current tank.\n"
	"Tracks the current tanks rotation.",
	"Look at the left of the current tank.\n"
	"Tracks the current tanks rotation.",
	"Look at the right of the current tank.\n"
	"Tracks the current tanks rotation.",
	"Look at the island from afar.",
	"A custom camera position has been made by the\n"
	"user."
};
static const int noCameraDescriptions = sizeof(cameraDescriptions) / sizeof(char *);

TargetCamera::TargetCamera() : 
	mainCam_(300, 300), 
	cameraPos_(CamSpectator), 
	totalTime_(0.0f),
	particleEngine_(&mainCam_, 6000),
	useHeightFunc_(true),
	dragging_(false),
	lastLandIntersectValid_(false)
{
	resetCam();

	mainCam_.setMinHeightFunc(minHeightFunc, this);
	mainCam_.setMaxHeightFunc(maxHeightFunc, this);
	DIALOG_ASSERT(noCameraDescriptions == noCameraNames);

	particleEngine_.setAllowSorting(false);
	rainEmitter_.setAttributes(
		4.0f, 4.0f, // Life
		0.5f, 0.5f, // Mass
		0.01f, 0.02f, // Friction
		Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
		Vector(1.0f, 1.0f, 1.0f), 0.6f, // StartColor1
		Vector(1.0f, 1.0f, 1.0f), 0.6f, // StartColor2
		Vector(1.0f, 1.0f, 1.0f), 0.6f, // EndColor1
		Vector(1.0f, 1.0f, 1.0f), 0.6f, // EndColor2
		0.2f, 0.2f, 0.2f, 0.2f, // Start Size
		0.2f, 0.2f, 0.2f, 0.2f, // EndSize
		Vector(0.0f, 0.0f, -1600.0f), // Gravity
		false,
		true);
	snowEmitter_.setAttributes(
		16.0f, 16.0f, // Life
		0.5f, 0.5f, // Mass
		0.01f, 0.02f, // Friction
		Vector(-10.0f, -10.0f, 0.0f), Vector(10.0f, 10.0f, 0.0f), // Velocity
		Vector(1.0f, 1.0f, 1.0f), 0.6f, // StartColor1
		Vector(1.0f, 1.0f, 1.0f), 0.6f, // StartColor2
		Vector(1.0f, 1.0f, 1.0f), 0.6f, // EndColor1
		Vector(1.0f, 1.0f, 1.0f), 0.6f, // EndColor2
		0.2f, 0.2f, 0.2f, 0.2f, // Start Size
		0.2f, 0.2f, 0.2f, 0.2f, // EndSize
		Vector(0.0f, 0.0f, -600.0f), // Gravity
		false,
		true);
}

TargetCamera::~TargetCamera()
{

}

void TargetCamera::resetCam()
{
	Vector at(128.0f, 128.0f, 5.0f);
	mainCam_.setLookAt(at, true);
	cameraPos_ = CamSpectator;
}

const char **TargetCamera::getCameraNames()
{
	return cameraNames;
}

ToolTip *TargetCamera::getCameraToolTips()
{
	if (cameraToolTips == 0)
	{
		cameraToolTips = new ToolTip[noCameraDescriptions];
		for (int i=0; i<noCameraDescriptions; i++)
		{
			cameraToolTips[i].setText(ToolTip::ToolTipHelp, 
				LANG_RESOURCE(getCameraNames()[i], getCameraNames()[i]),
				LANG_RESOURCE(std::string(getCameraNames()[i]) + "_camera", cameraDescriptions[i]));
		}
	}
	return cameraToolTips;
}

int TargetCamera::getNoCameraNames()
{
	return noCameraNames;
}

float TargetCamera::minHeightFunc(int x, int y, void *data)
{
	TargetCamera *instance = (TargetCamera *) data;
	if (instance->cameraPos_ == CamGun)
	{
		if ((instance->mainCam_.getCurrentPos() - 
			instance->mainCam_.getLookAt()).Magnitude() < 5.0f) return 0.0f;
	}

	const float heightMin = (Landscape::instance()->getWater().getWaterOn()?
		Landscape::instance()->getWater().getWaterHeight():0.0f);
	float addition = 5.0f;

	float h = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeight(x, y).asFloat() + addition;
	return (h>heightMin + addition?h:heightMin + addition);
}

float TargetCamera::maxHeightFunc(int x, int y, void *data)
{
	float h = 
		ScorchedClient::instance()->getLandscapeMaps().
			getRoofMaps().getRoofHeight(x, y).asFloat() - 2.0f;
	return h;
}

void TargetCamera::simulate(float frameTime, bool playing)
{
	totalTime_ += frameTime * ParticleEngine::getFast();
	while (totalTime_ > 0.05f)
	{
		if (!OptionsDisplay::instance()->getNoPrecipitation())
		{
			LandscapeTex &tex = *ScorchedClient::instance()->getLandscapeMaps().
				getDefinitions().getTex();
			if (tex.precipitation->getType() == LandscapeTexType::ePrecipitationRain)
			{
				LandscapeTexPrecipitation *rain = (LandscapeTexPrecipitation *)
					tex.precipitation;
				rainEmitter_.emitPrecipitation(mainCam_.getCurrentPos(),
					particleEngine_,
					rain->particles,
					true);
			}
			else if (tex.precipitation->getType() == LandscapeTexType::ePrecipitationSnow)
			{
				LandscapeTexPrecipitation *snow = (LandscapeTexPrecipitation *)
					tex.precipitation;
				snowEmitter_.emitPrecipitation(mainCam_.getCurrentPos(),
					particleEngine_,
					snow->particles,
					false);
			}
		}
		totalTime_ -= 0.1f;
	}

	particleEngine_.simulate(0, frameTime);
	if (moveCamera(frameTime, playing))
	{
		mainCam_.simulate(frameTime);
	}
}

void TargetCamera::draw()
{
	mainCam_.draw();
}

void TargetCamera::drawPrecipitation()
{
	particleEngine_.draw(0);
}

bool TargetCamera::moveCamera(float frameTime, bool playing)
{
	bool simulateCamera = true;
	Vector position(128.0f, 128.0f, 15.0f);
	float currentRotation = 0.0f;

	Tank *currentTank = ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (currentTank && 
		currentTank->getState().getState() == TankState::sNormal)
	{
		position = currentTank->getPosition().getTankTurretPosition().asVector();
		currentRotation = (180.0f - currentTank->getPosition().getRotationGunXY().asFloat()) / 57.32f;
	}

	bool viewFromBehindTank = false;
	switch (cameraPos_)
	{
	case CamAction:
		{
			CameraPositionAction *action = 
				CameraPositionActionRegistry::getCurrentAction();
			if (action)
			{
				mainCam_.setLookAt(action->getShowPosition().asVector());
				mainCam_.movePosition(currentRotation + 0.3f, 0.7f, 80.0f);
			}
			else viewFromBehindTank = true;
		}
		break;
	case CamTop:
		mainCam_.setLookAt(position);
		mainCam_.movePosition(currentRotation, 0.174f, 50.f);
		break;
	case CamTank:
		{
			Vector newPos(
				getFastSin(currentRotation) * 8.0f, 
				getFastCos(currentRotation) * 8.0f,
				0.0f);

			Vector newPos2 = position - newPos;
			mainCam_.setLookAt(newPos2);
			mainCam_.movePosition(currentRotation, 1.48f, 15.f);
		}
		break;
	case CamGun:
		if (ScorchedClient::instance()->getContext().getViewPoints().getLookAtCount() > 0)
		{
			FixedVector lookatPos, lookfromPos;
			ScorchedClient::instance()->getContext().getViewPoints().
				getValues(lookatPos, lookfromPos);

			mainCam_.setLookAt(lookatPos.asVector(), true);
			mainCam_.setOffSet(lookfromPos.asVector(), true);
			//simulateCamera = false;
		}
		else viewFromBehindTank = true;
		break;
	case CamBehind:
		{
			Vector newPos(
				getFastSin(currentRotation) * 25.0f, 
				getFastCos(currentRotation) * 25.0f,
				0.0f);
			Vector newPos2 = position - newPos;

			mainCam_.setLookAt(newPos2);
			mainCam_.movePosition(currentRotation, 1.0f, 60.f);
		}
		break;
	case CamLeftFar:
		{
			Vector newPos(
				getFastSin(currentRotation) * 65.0f, 
				getFastCos(currentRotation) * 65.0f,
				0.0f);
			Vector newPos2 = position - newPos;

			mainCam_.setLookAt(newPos2);
			mainCam_.movePosition(currentRotation + HALFPI, 1.0f, 100.f);
		}
		break;
	case CamRightFar:
		{
			Vector newPos(
				getFastSin(currentRotation) * 65.0f, 
				getFastCos(currentRotation) * 65.0f,
				0.0f);
			Vector newPos2 = position - newPos;

			mainCam_.setLookAt(newPos2);
			mainCam_.movePosition(currentRotation - HALFPI, 1.4f, 100.f);
		}
		break;
	case CamLeft: 
		{
			Vector newPos(
				getFastSin(currentRotation) * 10.0f, 
				getFastCos(currentRotation) * 10.0f,
				0.0f);
			Vector newPos2 = position - newPos;

			mainCam_.setLookAt(newPos2);
			mainCam_.movePosition(currentRotation + HALFPI, 1.4f, 20.f);
		}
		break;
	case CamRight:
		{
			Vector newPos(
				getFastSin(currentRotation) * 10.0f, 
				getFastCos(currentRotation) * 10.0f,
				0.0f);
			Vector newPos2 = position - newPos;

			mainCam_.setLookAt(newPos2);
			mainCam_.movePosition(currentRotation - HALFPI, 0.8f, 20.f);
		}
		break;
	case CamSpectator:
		{
			Vector at(128.0f, 128.0f, 0.0f);
			mainCam_.setLookAt(at);
			mainCam_.movePosition(HALFPI, 1.1f, 200.f);
		}
		break;
	default:
		break;
	}
	
	if (viewFromBehindTank)
	{
			if (playing &&
				currentTank && currentTank->getState().getState() == TankState::sNormal)
			{
				float currentElevation = (currentTank->getPosition().getRotationGunYZ().asFloat()) / 160.0f;
				Vector newPos = currentTank->getPosition().getTankGunPosition().asVector();
				Vector diff = newPos - position;
				Vector newPos2 = position + (diff);
				newPos2[2] += 0.5f;

				mainCam_.setLookAt(newPos2);
				mainCam_.movePosition(currentRotation, currentElevation + 1.57f, 3.0f);
			}
	}

	return simulateCamera;
}

bool TargetCamera::getLandIntersect(int x, int y, Vector &intersect)
{
	mainCam_.draw();
	Line direction;
	if (!mainCam_.getDirectionFromPt((float) x, (float) y, direction))
	{
		return false;
	}
	if (!ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().
		getIntersect(direction, intersect))
	{
		return false;
	}
	return true;
}

void TargetCamera::mouseDrag(GameState::MouseButton button, 
	int mx, int my, int x, int y, bool &skipRest)
{
	cameraPos_ = CamFree;
	if (button == GameState::MouseButtonRight)
	{
		if (OptionsDisplay::instance()->getInvertMouse())
		{
			y = -y;
		}

		const float QPI = 3.14f / 180.0f;
		mainCam_.movePositionDelta(
			(GLfloat) (x) * QPI,
			(GLfloat) (-y) * QPI,
			0.0f);
	}
	else if (button == GameState::MouseButtonLeft)
	{
		if (mx - dragXStart_ > 4 || mx - dragXStart_ < -4 ||
			my - dragYStart_ > 4 || my - dragYStart_ < -4)
		{
			dragging_ = true;
		}

		if (dragging_)
		{
			cameraPos_ = CamFree;
			float arenaWidth = (float) ScorchedClient::instance()->getLandscapeMaps().
				getGroundMaps().getArenaWidth();
			float arenaHeight = (float) ScorchedClient::instance()->getLandscapeMaps().
				getGroundMaps().getArenaHeight();
			float arenaX = (float) ScorchedClient::instance()->getLandscapeMaps().
				getGroundMaps().getArenaX();
			float arenaY = (float) ScorchedClient::instance()->getLandscapeMaps().
				getGroundMaps().getArenaY();
			mainCam_.scroll(float(-x / 2), float(-y / 2), 
				arenaX, arenaY, arenaX + arenaWidth, arenaY + arenaHeight);
		}
	}
	else
	{
		mainCam_.movePositionDelta(
			0.0f, 0.0f,
			(GLfloat) (y));
	}
}

void TargetCamera::mouseWheel(int x, int y, int z, bool &skipRest)
{
	cameraPos_ = CamFree;
	mainCam_.movePositionDelta(
			0.0f, 0.0f,
			((GLfloat) z) / 10.0f);
}

void TargetCamera::mouseDown(GameState::MouseButton button, 
	int x, int y, bool &skipRest)
{
	dragXStart_ = x;
	dragYStart_ = y;

	lastLandIntersectValid_ = 
		getLandIntersect(x, y, lastLandIntersect_);
}

void TargetCamera::mouseUp(GameState::MouseButton button, 
	int x, int y, bool &skipRest)
{
	// Check if we were dragging with the left mouse button
	// If we were then we ignore the request as the user will have already
	// moved the camera using dragging, not using clicking
	if (dragging_)
	{
		dragging_ = false;
		return;
	}

	// Check the intersection point was valid
	if (!lastLandIntersectValid_)
	{
		return;
	}
	lastLandIntersectValid_ = false;

    skipRest = true;

	// Does the click on the landscape mean we move there
	// or do we just want to look there
	// This depends on the mode the current weapon is
	Accessory *currentWeapon = 0;
	Tank *currentTank = 0;
	Accessory::PositionSelectType selectType = Accessory::ePositionSelectNone;
	if (ScorchedClient::instance()->getGameState().getState() == 
		ClientState::StatePlaying)
	{
		currentTank = ScorchedClient::instance()->
			getTankContainer().getCurrentTank();
		if (currentTank)
		{
			currentWeapon = currentTank->getAccessories().getWeapons().getCurrent();
			if (currentWeapon)
			{
				selectType = currentWeapon->getPositionSelect();
			}
		}
	}

	// Try to move the tank to the position on the landscape
	int arenaWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getArenaWidth();
	int arenaHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getArenaHeight();
	int arenaX = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getArenaX();
	int arenaY = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getArenaY();
	int posX = (int) lastLandIntersect_[0];
	int posY = (int) lastLandIntersect_[1];
	if (posX > arenaX && posX < arenaX + arenaWidth &&
		posY > arenaY && posY < arenaY + arenaHeight)
	{
		// Just look at the point on the landscape
		if (selectType == Accessory::ePositionSelectNone)
		{
			cameraPos_ = CamFree;
			mainCam_.setLookAt(lastLandIntersect_);
			return;
		}

		if (selectType == Accessory::ePositionSelectFuel)
		{
			WeaponMoveTank *moveWeapon = (WeaponMoveTank *)
				ScorchedClient::instance()->getAccessoryStore().
					findAccessoryPartByAccessoryId(
					currentWeapon->getAccessoryId(), "WeaponMoveTank");
			if (!moveWeapon) return;

			MovementMap mmap(
				currentTank,
				ScorchedClient::instance()->getContext());

			FixedVector pos((int) posX, (int) posY, 0);
			mmap.calculatePosition(pos, mmap.getFuel(moveWeapon));

			MovementMap::MovementMapEntry &entry =	mmap.getEntry(posX, posY);
			if (entry.type != MovementMap::eMovement) return;  // Do nothing
		}
		else if (selectType == Accessory::ePositionSelectFuelLimit)
		{
			int limit = currentWeapon->getPositionSelectLimit();
			MovementMap mmap(
				currentTank,
				ScorchedClient::instance()->getContext());

			FixedVector pos((int) posX, (int) posY, 0);
			mmap.calculatePosition(pos, fixed(limit));

			MovementMap::MovementMapEntry &entry =	mmap.getEntry(posX, posY);
			if (entry.type != MovementMap::eMovement) return;  // Do nothing
		}
		else if (selectType == Accessory::ePositionSelectLimit)
		{
			int limit = currentWeapon->getPositionSelectLimit();
			FixedVector position(posX, posY, 0);
			if ((currentTank->getLife().getTargetPosition() - position).Magnitude() > limit)
			{
				// Out of limit
				return;
			}
		}

		currentTank->getPosition().setSelectPosition(posX, posY);
		TankKeyboardControlUtil::fireShot(currentTank);
	}
}

void TargetCamera::keyboardCheck(float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, 
							   int hisCount, 
							   bool &skipRest)
{
	KEYBOARDKEY("CAMERA_TOP_VIEW", topViewKey);
	KEYBOARDKEY("CAMERA_BEHIND_VIEW", behindViewKey);
	KEYBOARDKEY("CAMERA_TANK_VIEW", tankViewKey);
	KEYBOARDKEY("CAMERA_GUN_VIEW", gunViewKey);
	KEYBOARDKEY("CAMERA_ACTION_VIEW", actionViewKey);
	KEYBOARDKEY("CAMERA_LEFT_VIEW", leftViewKey);
	KEYBOARDKEY("CAMERA_RIGHT_VIEW", rightViewKey);
	KEYBOARDKEY("CAMERA_LEFTFAR_VIEW", leftFarViewKey);
	KEYBOARDKEY("CAMERA_RIGHTFAR_VIEW", rightFarViewKey);
	KEYBOARDKEY("CAMERA_SPECTATOR_VIEW", spectatorViewKey);

	if (topViewKey->keyDown(buffer, keyState)) cameraPos_ = CamTop;
	else if (behindViewKey->keyDown(buffer, keyState)) cameraPos_ = CamBehind;
	else if (tankViewKey->keyDown(buffer, keyState)) cameraPos_ = CamTank;
	else if (gunViewKey->keyDown(buffer, keyState)) cameraPos_ = CamGun;
	else if (leftViewKey->keyDown(buffer, keyState)) cameraPos_ = CamLeft;
	else if (rightViewKey->keyDown(buffer, keyState)) cameraPos_ = CamRight;
	else if (leftFarViewKey->keyDown(buffer, keyState)) cameraPos_ = CamLeftFar;
	else if (rightFarViewKey->keyDown(buffer, keyState)) cameraPos_ = CamRightFar;
	else if (spectatorViewKey->keyDown(buffer, keyState)) cameraPos_ = CamSpectator;
	else if (actionViewKey->keyDown(buffer, keyState)) cameraPos_ = CamAction;

	const float QPI = 3.14f / 4.0f;
	KEYBOARDKEY("CAMERA_ROTATE_LEFT", leftKey);
	KEYBOARDKEY("CAMERA_ROTATE_RIGHT", rightKey);
	if (leftKey->keyDown(buffer, keyState))
	{
		cameraPos_ = CamFree;
		mainCam_.movePositionDelta(QPI * frameTime, 0.0f, 0.0f);
	}
	else if (rightKey->keyDown(buffer, keyState))
	{
		cameraPos_ = CamFree;
		mainCam_.movePositionDelta(-QPI * frameTime, 0.0f, 0.0f);
	}

	KEYBOARDKEY("CAMERA_ROTATE_DOWN", downKey);
	KEYBOARDKEY("CAMERA_ROTATE_UP", upKey);
	if (upKey->keyDown(buffer, keyState))
	{
		cameraPos_ = CamFree;
		mainCam_.movePositionDelta(0.0f, -QPI * frameTime, 0.0f);
	}
	else if (downKey->keyDown(buffer, keyState))
	{
		cameraPos_ = CamFree;
		mainCam_.movePositionDelta(0.0f, QPI * frameTime, 0.0f);
	}

	KEYBOARDKEY("CAMERA_ZOOM_IN", inKey);
	KEYBOARDKEY("CAMERA_ZOOM_OUT", outKey);
	if (inKey->keyDown(buffer, keyState))
	{
		cameraPos_ = CamFree;
		mainCam_.movePositionDelta(0.0f, 0.0f, -100.0f * frameTime);
	}
	else if (outKey->keyDown(buffer, keyState))
	{
		cameraPos_ = CamFree;
		mainCam_.movePositionDelta(0.0f, 0.0f, 100.0f * frameTime);
	}

	KEYBOARDKEY("CAMERA_NOLIMIT", limitKey);
	if (limitKey->keyDown(buffer, keyState, false))
	{	
		useHeightFunc_ = !useHeightFunc_;
		ChannelManager::showText(
			ScorchedClient::instance()->getContext(), 
			ChannelText("info", S3D::formatStringBuffer("Restricted camera movement : %s", 
			(useHeightFunc_?"On":"Off"))));
	}
	mainCam_.setUseHeightFunc(useHeightFunc_);
}
