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

#include <GLW/GLWPlanView.h>
#include <GLW/GLWTranslate.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLViewPort.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <graph/MainCamera.h>
#include <client/ClientLinesHandler.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <common/Vector.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsTransient.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsLinesMessage.h>
#include <graph/OptionsDisplay.h>
#include <math.h>

REGISTER_CLASS_SOURCE(GLWPlanView);

static const float degToRad = 3.14f / 180.0f;
static const float maxAnimationTime = 2.0f;

GLWPlanView::GLWPlanView(float x, float y, float w, float h) :
	GLWidget(x, y, w, h),
	animationTime_(0.0f), flashTime_(0.0f), totalTime_(0.0f), pointTime_(0.0f),
	flash_(true), dragging_(false), firstTime_(true),
	planColor_(1.0f)
{
	setToolTip(new ToolTip(ToolTip::ToolTipHelp, "Plan View",
		"Shows the position of the the tanks\n"
		"on a overhead map of the island.\n"
		"Flashing tanks are still to make a move.\n"
		"Clicking on the plan will move the camera\n"
		"to look at that point."));
}

GLWPlanView::~GLWPlanView()
{
}

void GLWPlanView::simulate(float frameTime)
{
	planColor_ += frameTime * 0.2f;
	if (planColor_ > 1.0f) planColor_ = 1.0f;

	totalTime_ += frameTime;
	flashTime_ += frameTime;
	if (flashTime_ > 0.3f)
	{
		flashTime_ = 0.0f;
		flash_ = !flash_;
	}

	animationTime_ += frameTime;
	if (animationTime_ > maxAnimationTime)
	{
		animationTime_ = 0.0f;
	}

	// Send messages about lines that have been drawn once every
	// 2 seconds
	pointTime_ += frameTime;
	if (pointTime_ > 2.0f)
	{
		pointTime_ = 0.0f;

		if (!sendPoints.empty())
		{
			Vector last = sendPoints.back();
			last[2] = 0.0f;

			ComsLinesMessage message(
				ScorchedClient::instance()->getTankContainer().getCurrentPlayerId());
			message.getLines() = sendPoints;
			if (dragging_) message.getLines().push_back(Vector::getNullVector());
			sendPoints.clear();
			ComsMessageSender::sendToServer(message);

			if (dragging_) sendPoints.push_back(last);
		}
	}

	// Simulate all points
	simulateLine(localPoints_); // Local
	std::list<PlayerDrawnInfo>::iterator playeritor; // Remote
	for (playeritor = dragPoints_.begin();
		playeritor != dragPoints_.end();
		)
	{
		PlayerDrawnInfo &info = (*playeritor);
		std::list<Vector>::iterator recieveitor;
		for (recieveitor = info.recievepoints.begin();
			recieveitor != info.recievepoints.end();
			recieveitor++)
		{
			Vector &v = (*recieveitor);
			if (v[2] > 3.0f) v[2] = 3.0f;
			v[2] -= frameTime; 
		}

		if (!simulateLine(info))
		{
			playeritor = dragPoints_.erase(playeritor);
		}
		else
		{
			playeritor++;
		}
	}
}

void GLWPlanView::draw()
{
	if (firstTime_)
	{
		firstTime_ = false;
		if (!OptionsDisplay::instance()->getNoPlanDraw())
		{
			ClientLinesHandler::instance()->registerCallback(this);
		}
	}

	GLWidget::draw();
	drawMap();
}

void GLWPlanView::drawMap()
{
	GLState currentState(GLState::DEPTH_OFF | GLState::BLEND_ON | GLState::TEXTURE_ON);

	glPushMatrix();
		glTranslatef(x_ + 2.0f, y_ + 2.0f, 0.0f);
		glScalef(w_ - 4.0f, h_ - 4.0f, 1.0f);

		drawTexture();
		{
			GLState currentState2(GLState::TEXTURE_OFF);
			drawCameraPointer();
			drawTanks();
			drawLines();
			drawBuoys();
		}
	glPopMatrix();
}

void GLWPlanView::drawLines()
{
	glLineWidth(2.0f);
	if (!localPoints_.points.empty())
	{
		localPoints_.playerId = ScorchedClient::instance()->
			getTankContainer().getCurrentPlayerId();
		drawLine(localPoints_);
	}

	if (!dragPoints_.empty())
	{
		std::list<PlayerDrawnInfo>::iterator playeritor;
		for (playeritor = dragPoints_.begin();
			playeritor != dragPoints_.end();
			playeritor++)
		{
			PlayerDrawnInfo &info = (*playeritor);
			drawLine(info);
		}
	}
	glLineWidth(1.0f);
}

bool GLWPlanView::simulateLine(PlayerDrawnInfo &info)
{
	while (!info.recievepoints.empty())
	{
		Vector &first = info.recievepoints.front();
		if (first[2] > 0.0f) break;
		first[2] = totalTime_;
		info.points.push_back(first);
		info.recievepoints.pop_front();
	}

	while (!info.points.empty())
	{
		Vector &first = info.points.front();
		float time = totalTime_ - first[2];
		if (time < 3.0f) break;
		info.points.pop_front();
	}

	return !(info.points.empty() && info.recievepoints.empty());
}

void GLWPlanView::drawLine(PlayerDrawnInfo &info)
{
	Tank *current = 
		ScorchedClient::instance()->getTankContainer().getTankById(
			info.playerId);
	if (!current)
	{
		info.points.clear();
		info.recievepoints.clear();
		return;
	}

	if (info.points.empty()) return;

	glBegin(GL_LINE_STRIP);
	std::list<Vector>::iterator itor;
	for (itor = info.points.begin();
		itor != info.points.end();
		itor++)
	{
		Vector &v = (*itor);

		if (v[0] == 0.0f && v[1] == 0.0f)
		{
			glEnd();
			glBegin(GL_LINE_STRIP);
		}
		else
		{
			if (v[0] >= 0.0f && v[1] >= 0.0f &&
				v[0] <= 1.0f && v[1] <= 1.0f)
			{
				float time = totalTime_ - v[2];
				time = 1.0f - (time / 3.0f);
				glColor4f(
					current->getColor()[0],
					current->getColor()[1], 
					current->getColor()[2], 
					time);
				glVertex2f(v[0], v[1]);
			}
		}
	}
	glEnd();
}

void GLWPlanView::drawTexture()
{
	// Draw the camera pointer
	float mapWidth = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getMapWidth();
	float mapHeight = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getMapHeight();
	float maxWidth = MAX(mapWidth, mapHeight);

	Vector lowerLeft(0.0f, 0.0f, 0.0f);
	Vector upperRight(mapWidth / maxWidth, mapHeight / maxWidth, 0.0f);
	lowerLeft[0] += (maxWidth - mapWidth) / maxWidth / 2.0f;
	upperRight[0] += (maxWidth - mapWidth) / maxWidth / 2.0f;
	lowerLeft[1] += (maxWidth - mapHeight) / maxWidth / 2.0f;
	upperRight[1] += (maxWidth - mapHeight) / maxWidth / 2.0f;

	// Draw the square of land
	glColor3f(planColor_, planColor_, planColor_);
	Landscape::instance()->getPlanATexture().draw(true);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(upperRight[0], lowerLeft[1]);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(upperRight[0], upperRight[1]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(lowerLeft[0], upperRight[1]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(lowerLeft[0], lowerLeft[1]);
	glEnd();
}

void GLWPlanView::drawCameraPointer()
{
	// Draw the camera pointer
	float mapWidth = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getMapWidth();
	float mapHeight = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getMapHeight();
	float maxWidth = MAX(mapWidth, mapHeight);

	Vector mid(0.5f, 0.5f);
	// Get camera positions
	Vector currentPos = MainCamera::instance()->
		getCamera().getCurrentPos();
	Vector lookAt = MainCamera::instance()->
		getCamera().getLookAt();
	Vector direction = (currentPos - lookAt).Normalize2D() * 0.2f;
	Vector directionPerp = direction.get2DPerp();

	// Force camera positions between 0 and 1
	currentPos[0] += (maxWidth - mapWidth) / 2.0f;
	currentPos[1] += (maxWidth - mapHeight) / 2.0f;
	currentPos[0] /= maxWidth;
	currentPos[1] /= maxWidth;
	currentPos[2] = 0.0f;
	lookAt[0] += (maxWidth - mapWidth) / 2.0f;
	lookAt[1] += (maxWidth - mapHeight) / 2.0f;
	lookAt[0] /= maxWidth;
	lookAt[1] /= maxWidth;
	lookAt[2] = 0.0f;

	// Make sure currentpos doesn't go outside boundries
	currentPos -= mid;
	float distance = currentPos.Magnitude();
	if (distance > 0.5f)
	{
		currentPos *= 0.5f / distance;
	}
	currentPos += mid;

	// Make sure look at doesn't go outside boundries
	lookAt -= mid;
	distance = lookAt.Magnitude();
	if (distance > 0.48f)
	{
		lookAt *= 0.48f / distance;
	}
	lookAt += mid;

	// Draw direction arrows
	glColor3f(0.9f, 0.9f, 1.0f);
	glBegin(GL_LINES);
		glVertex3fv(currentPos);
		glVertex3fv(currentPos - (direction + directionPerp) / 2.0f);
		glVertex3fv(currentPos);
		glVertex3fv(currentPos - (direction - directionPerp) / 2.0f);
		glVertex3fv(currentPos - (direction + directionPerp) / 4.0f);
		glVertex3fv(currentPos - (direction - directionPerp) / 4.0f);
	glEnd();

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x0F0F);
	glColor3f(0.7f, 0.7f, 0.7f);
	glBegin(GL_LINES);
		glVertex3fv(currentPos);
		glVertex3fv(lookAt);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
}

void GLWPlanView::drawBuoys()
{
	//Get the wall type and set the colour accordingly
	switch(ScorchedClient::instance()->getOptionsTransient().getWallType())
	{
	case OptionsTransient::wallWrapAround:
		glColor3f(0.5f, 0.5f, 0.0f);	// Keep the colours dark on the outside
		break;				// to try to give a look of shape
	case OptionsTransient::wallBouncy:
		glColor3f(0.0f, 0.0f, 0.5f);
		break;
	case OptionsTransient::wallConcrete:
		glColor3f(0.2f, 0.2f, 0.2f);
		break;
	case OptionsTransient::wallNone:
		return;
	default:
		break;	// should never happen....
	}

	int mapWidth = ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getMapWidth();
	int mapHeight = ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getMapHeight();
	int maxWidth = MAX(mapWidth, mapHeight);
	
	// Note: Buoys are placed every 64 units, stretched up to fit
	// ie: a 256 map will have 4, a 240 will have three
	//
	// Figure out how many bouys
	int pointsX = mapWidth / 64;	// Each point is 64 units apart, as per landscape/LandscapePoints.cpp
	int pointsY = mapHeight / 64;

	// Get the scaling factor for non-square maps
	float xscale = (float) mapWidth / (float) maxWidth;
	float yscale = (float) mapHeight / (float) maxWidth;


	int i;
	std::vector<Vector> points;
	// Generate the points (not scaled yet)
	for (i=0; i<pointsX; i++)
	{
		float pos = 1.0f / float(pointsX - 1) * float(i);

		points.push_back(Vector(pos, 0.0f));
		points.push_back(Vector(pos, float(mapHeight)));
	}
	for (i=0; i<pointsY; i++)
	{
		float pos = 1.0f / float(pointsY - 1) * float(i);

		points.push_back(Vector(0.0f, pos));
		points.push_back(Vector(float(mapWidth), pos));
	}

	// Plot the dots, scaling for non-square maps
	// Draw the dots!
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (int a=0; a<2; a++)
	{
		for (int i=1; i<(int) points.size(); i++)
		{
			glVertex3f( (1.0f - xscale) / 2.0f + points[i][0] * xscale, 
				(1.0f - yscale) / 2.0f + points[i][1] * yscale, 0.0f);
		}
		for (int i=1; i<(int) points.size(); i++)
		{
			glVertex3f( 1.0f - ((1.0f - xscale) / 2.0f + points[i][0] * xscale), 
				1.0f - ((1.0f - yscale) / 2.0f + points[i][1] * yscale), 0.0f);
		}

		if (a == 0)
		{
			glEnd();	

			switch(ScorchedClient::instance()->getOptionsTransient().getWallType())
			{
			case OptionsTransient::wallWrapAround:
				glColor3f(0.9f, 0.9f, 0.4f);	// Keep the colours dark on the outside
				break;				// to try to give a look of shape
			case OptionsTransient::wallBouncy:
				glColor3f(0.4f, 0.4f, 0.4f);
				break;
			case OptionsTransient::wallConcrete:
				glColor3f(0.6f, 0.6f, 0.6f);
				break;
			default:
				break;	// should never happen....
			}

			glPointSize(2.0f);
			glBegin(GL_POINTS);
		}
	}
	glEnd();

	glDisable(GL_POINT_SMOOTH);
	glPointSize(1.0f);
}
void GLWPlanView::drawTanks()
{
	std::map<unsigned int, Tank *> &currentTanks = 
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	if (currentTanks.empty()) return;

	// Draw the current tank bolder
	float mapWidth = (float) 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getMapWidth();
	float mapHeight = (float) 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getMapHeight();
	float maxWidth = MAX(mapWidth, mapHeight);

	Vector position;
	drawCurrentTank();

	// Draw the rest of the tanks
	// as points on the plan view
	glEnable(GL_POINT_SMOOTH);

	glPointSize(7.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = currentTanks.begin();
		itor != currentTanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getState().getState() == TankState::sNormal &&
			!tank->getState().getSpectator())
		{
			position = tank->getPosition().getTankPosition().asVector();
			position[0] += (maxWidth - mapWidth) / 2.0f;
			position[1] += (maxWidth - mapHeight) / 2.0f;
			position /= maxWidth;

			glVertex3fv(position);
		}
	}
	glEnd();

	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (itor = currentTanks.begin();
		itor != currentTanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getState().getState() == TankState::sNormal &&
			!tank->getState().getSpectator())
		{
			glColor3fv(tank->getColor());
			position = tank->getPosition().getTankPosition().asVector();
			position[0] += (maxWidth - mapWidth) / 2.0f;
			position[1] += (maxWidth - mapHeight) / 2.0f;
			position /= maxWidth;

			if ((flash_ && tank->getState().getReadyState() == TankState::SNotReady) ||
				tank->getState().getReadyState() == TankState::sReady)
			{
				glVertex3fv(position);
			}

			TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
				tank->getRenderer();
			if (renderer)
			{
				GLWToolTip::instance()->addToolTip(&renderer->getTips()->tankTip,
					GLWTranslate::getPosX() + x_ + position[0] * w_ - 4.0f, 
					GLWTranslate::getPosY() + y_ + position[1] * h_ - 4.0f, 
					8.0f, 8.0f);
			}
		}
	}
	glEnd();
	glDisable(GL_POINT_SMOOTH);
}

void GLWPlanView::drawCurrentTank()
{
	// Draw the extra rings around the currently playing player
	// on the plan view

	// Check that the current player should be shown on the plan
	Tank *currentTank = ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (!currentTank || 
		currentTank->getState().getState() != TankState::sNormal) return;
	if (ScorchedClient::instance()->getGameState().getState() != 
		ClientState::StatePlaying) return;

	// Calculate the constants
	float mapWidth = (float) 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getMapWidth();
	float mapHeight = (float) 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getMapHeight();
	float maxWidth = MAX(mapWidth, mapHeight);
	float scale = animationTime_ / maxAnimationTime;
	Vector position;
	float rot = -currentTank->getPosition().getRotationGunXY().asFloat() * degToRad;
	position = currentTank->getPosition().getTankPosition().asVector();
	position[0] += (maxWidth - mapWidth) / 2.0f;
	position[1] += (maxWidth - mapHeight) / 2.0f;
	position /= maxWidth;

	// Build a display list to build the ring
	// only once
	static GLuint listNo = 0;
	if (!listNo)
	{
		glNewList(listNo = glGenLists(1), GL_COMPILE);
			glPointSize(1.0f);
			glBegin(GL_LINES);
				for (float a=0; a<=6.28; a+=0.4f)
				{
					glVertex2f(sinf(a) * 0.07f, cosf(a) * 0.07f);
				}
			glEnd();
		glEndList();
	}

	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);

		// Draw a expanding ring around the current tank
		glColor3fv(currentTank->getColor());
		glPushMatrix();
			glScalef(scale, scale, scale);
			glCallList(listNo);
		glPopMatrix();

		// Draw a line pointing where the current tank is looking
		glBegin(GL_LINES);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(
				getFastSin(rot) * 0.07f, 
				getFastCos(rot) * 0.07f);
		glEnd();
	glPopMatrix();

}

void GLWPlanView::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		if (button == GameState::MouseButtonLeft)
		{
			float mapWidth = (float) ScorchedClient::instance()->
				getLandscapeMaps().getGroundMaps().getMapWidth();
			float mapHeight = (float) ScorchedClient::instance()->
				getLandscapeMaps().getGroundMaps().getMapHeight();
			float maxWidth = MAX(mapWidth, mapHeight);

			float mapX = ((x - x_) / w_) * maxWidth;
			float mapY = ((y - y_) / h_) * maxWidth;
			mapX -= (maxWidth - mapWidth) / 2.0f;
			mapY -= (maxWidth - mapHeight) / 2.0f;

			Vector lookAt(mapX, mapY, ScorchedClient::instance()->
				getLandscapeMaps().getGroundMaps().getInterpHeight(
					fixed::fromFloat(mapX), fixed::fromFloat(mapY)).asFloat() + 5.0f);
			MainCamera::instance()->getTarget().setCameraType(TargetCamera::CamFree);
			MainCamera::instance()->getCamera().setLookAt(lookAt);
		}
		else if (button == GameState::MouseButtonRight)
		{
			dragging_ = true;

			dragLastX_ = x;
			dragLastY_ = y;

			float mapX = ((x - x_) / w_);
			float mapY = ((y - y_) / h_);
			localPoints_.points.push_back(Vector(mapX, mapY, totalTime_));
			sendPoints.push_back(Vector(mapX, mapY, pointTime_));
		}
	}
}

void GLWPlanView::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (dragging_)
	{
		skipRest = true;
		if (inBox(mx, my, x_, y_, w_, h_))
		{
			if (fabsf(mx - dragLastX_) > 5.0f ||
				fabsf(my - dragLastY_) > 5.0f)
			{
				dragLastX_ = mx;
				dragLastY_ = my;

				float mapX = ((mx - x_) / w_);
				float mapY = ((my - y_) / h_);
				localPoints_.points.push_back(Vector(mapX, mapY, totalTime_));
				sendPoints.push_back(Vector(mapX, mapY, pointTime_));
			}
		}
	}
}

void GLWPlanView::mouseUp(int button, float x, float y, bool &skipRest)
{
	if (dragging_)
	{
		dragging_ = false;
		localPoints_.points.push_back(Vector::getNullVector());
		sendPoints.push_back(Vector::getNullVector());
	}
}

void GLWPlanView::addRecievePoints(unsigned int playerId, 
	std::list<Vector> &recievepoints)
{
	PlayerDrawnInfo *foundInfo = 0;
	std::list<PlayerDrawnInfo>::iterator dragItor;
	for (dragItor = dragPoints_.begin();
		dragItor != dragPoints_.end();
		dragItor++)
	{
		PlayerDrawnInfo &info = (*dragItor);
		if (info.playerId == playerId)
		{
			foundInfo = &info;
		}
	}
	if (!foundInfo)
	{
		planColor_ = 0.2f;

		dragPoints_.push_back(PlayerDrawnInfo());
		foundInfo = &dragPoints_.back();
		foundInfo->playerId = playerId;
	}
	
	foundInfo->recievepoints.insert(foundInfo->recievepoints.end(),
		recievepoints.begin(), recievepoints.end());
}
