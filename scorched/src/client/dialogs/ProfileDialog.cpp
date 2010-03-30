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

#include <dialogs/ProfileDialog.h>
#include <GLEXT/GLViewPort.h>
#include <GLEXT/GLState.h>
#include <GLW/GLWFont.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <graph/Main2DCamera.h>
#include <tank/TankContainer.h>
#include <tank/TankPosition.h>
#include <landscapemap/LandscapeMaps.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <sprites/ExplosionTextures.h>
#include <XML/XMLNode.h>

ProfileDialog *ProfileDialog::instance_ = 0;

ProfileDialog *ProfileDialog::instance()
{
	if (!instance_)
	{
		instance_ = new ProfileDialog;
	}

	return instance_;
}

ProfileDialog::ProfileDialog() : 
	GLWWindow("Profile", 200, 300, 640, 200, eTransparent | 
	eResizeable | eSmallTitle | eSavePosition,
	"Shows a 2D side view of the landscape and tanks"),
	profileZoom_(1.0f),
	zooming_(false)
{
}

ProfileDialog::~ProfileDialog()
{
}

void ProfileDialog::saveSettings(XMLNode *node)
{
	GLWWindow::saveSettings(node);

	node->addChild(new XMLNode("w", int(getW())));
	node->addChild(new XMLNode("h", int(getH())));
}

void ProfileDialog::loadSettings(XMLNode *node, bool resetPositions)
{
	GLWWindow::loadSettings(node, resetPositions);

	if (!resetPositions)
	{
		int w, h;
		if (node->getNamedChild("w", w, false)) setW(float(w));
		if (node->getNamedChild("h", h, false)) setH(float(h));
	}
}

void ProfileDialog::draw()
{
	GLWWindow::draw();
	
	// Set the new viewport
	const int inset = 8;
	int windowWidth = int(float(w_ - inset - inset) / GLViewPort::getWidthMult());
	int windowHeight = int(float(h_ - inset - inset) / GLViewPort::getHeightMult()); 
	int windowX = int(float(x_ + inset) / GLViewPort::getWidthMult()); 
	int windowY = int(float(y_ + inset) / GLViewPort::getHeightMult());
	
	float ox = 0.0f;
	float oy = 0.0f;
	float ow = w_ / 2.0f;
	float oh = h_ / 2.0f;

	if (profileZoom_ > 1.0f)
	{
		float xper = (zoomX_ - (x_ + inset)) / (w_ - inset - inset);
		float yper = (zoomY_ - (y_ + inset)) / (h_ - inset - inset);
		float x = xper * ow;
		float y = yper * oh;
		ox = (ox - x) / profileZoom_ + x;
		oy = (oy - y) / profileZoom_ + y;
		ow = (ow - x) / profileZoom_ + x;
		oh = (oh - y) / profileZoom_ + y;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glViewport(windowX, windowY, windowWidth, windowHeight);
	glOrtho(ox, ow, oy, oh, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Draw the stuff we want to see
	Tank *currentTank =
		ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (currentTank && currentTank->getAlive())
	{
		drawAiming(currentTank);
		drawLandscape(currentTank);
		drawTanks(currentTank);
	}
		
	// Return the viewport to the original
	Main2DCamera::instance()->draw(0);
}

void ProfileDialog::simulate(float frameTime)
{
	if (zooming_) profileZoom_ += frameTime * 4.0f;
	else profileZoom_ -= frameTime * 4.0f;

	if (profileZoom_ < 1.0f) profileZoom_ = 1.0f;
	else if (profileZoom_ > 2.0f) profileZoom_ = 2.0f;
}

void ProfileDialog::drawLandscape(Tank *currentTank)
{
	// Get tank attributes
	fixed tankRotation = -currentTank->getPosition().getRotationGunXY() / 180 * fixed::XPI;
	FixedVector tankDirection(tankRotation.sin(), tankRotation.cos(), 0);
	FixedVector startPosition = currentTank->getPosition().getTankPosition() - tankDirection * 25;

	// Draw the landscape
	GLState state1(GLState::TEXTURE_OFF | GLState::BLEND_ON | GLState::DEPTH_OFF);
	glColor4f(0.0f, 0.5f, 0.0f, 1.0f);

	glBegin(GL_QUAD_STRIP);
	FixedVector position = startPosition;
	float x = 0.0f;
	for (int i=0; i<=400; i++)
	{
		float height =
			ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getInterpHeight(
			position[0], position[1]).asFloat();
		if (height < 1.0f) height = 1.0f;

		glVertex2f(x, height);
		glVertex2f(x, 0.0f);

		x += 1.0f;
		position += tankDirection;
	}
	glEnd();
}

void ProfileDialog::drawTanks(Tank *currentTank)
{
	// Get tank attributes
	FixedVector tankPosition = currentTank->getPosition().getTankPosition();
	fixed tankRotation = -currentTank->getPosition().getRotationGunXY() / 180 * fixed::XPI;
	FixedVector tankDirection(tankRotation.sin(), tankRotation.cos(), 0);
	FixedVector startPosition = tankPosition - tankDirection * 25;

	GLState state2(GLState::TEXTURE_ON | GLState::BLEND_ON);

	FixedVector tankWindowPos;
	std::map<unsigned int, Tank *> &tanks =
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin(); 
		itor != tanks.end();
		itor++)
	{
		Tank *tank = itor->second;
		if (!tank->getAlive()) continue;

		if (tank != currentTank)
		{
			FixedVector thisTankDir = (tank->getPosition().getTankPosition() - tankPosition);
			thisTankDir[2] = 0;
			thisTankDir.StoreNormalize();
			fixed dotP = thisTankDir.dotP2D(tankDirection);
			if (dotP < fixed(true, 8000)) continue;
		}

		FixedVector AP = tank->getPosition().getTankPosition() - startPosition;
		FixedVector AB = tankDirection * 400;

		fixed ab2 = AB[0]*AB[0] + AB[1]*AB[1];
		fixed ap_ab = AP[0]*AB[0] + AP[1]*AB[1];
		fixed t = ap_ab / ab2;
		if (t>=0 && t<1)
		{
			FixedVector point = startPosition + AB * t;
			fixed landheight =
				ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getInterpHeight(
				point[0], point[1]);
			fixed x = t * 400;
			fixed tankheight = tank->getPosition().getTankPosition()[2];

			if (landheight < tankheight + 10)
			{
				landheight = tankheight + 10;
			}

			TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
				tank->getRenderer();
			if (renderer)
			{
				// Draw the tank
				glPushMatrix();
					// Set the tank angle
					glTranslatef(x.asFloat(), tankheight.asFloat(), 0.0f);
					glScalef(10.0f, 10.0f, 10.0f);

					Vector position;
					TankMesh *mesh = renderer->getMesh();
					if (mesh)
					{
						glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
						Vector4 rotation;
						Vector axis(1.0f, 0.0f, 0.0f);
						rotation.setQuatFromAxisAndAngle(axis, 0.0f);
						glScalef(0.5f, 0.5f, 0.5f);

						float matrix[16];
						rotation.getOpenGLRotationMatrix(matrix);

						mesh->draw(
							0.0f,
							matrix, position, 0.0f,
							tank->getPosition().getRotationGunXY().asFloat(),
							tank->getPosition().getRotationGunYZ().asFloat(),
							false);
					}
				glPopMatrix();
			}
						
			ExplosionTextures::instance()->arrowTexture.draw();
			glColor3fv(tank->getColor());
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(x.asFloat() - 2.5f, landheight.asFloat() + 10.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(x.asFloat() - 2.5f, landheight.asFloat() + 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(x.asFloat() + 2.5f, landheight.asFloat() + 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(x.asFloat() + 2.5f, landheight.asFloat() + 10.0f);
			glEnd();

			glDepthMask(GL_FALSE);
			float textWidth =
				GLWFont::instance()->getGameFont()->getWidth(5.0f,
					tank->getTargetName());
			GLWFont::instance()->getGameFont()->draw(
				tank->getColor(), 5.0f, 
				x.asFloat() - textWidth / 2.0f, 
				landheight.asFloat() + 12.0f, 
				0.0f,
				tank->getTargetName());
			glDepthMask(GL_TRUE);

			if (tank == currentTank) tankWindowPos = FixedVector(x, tankheight, 0);
		}
	}
}

void ProfileDialog::drawAiming(Tank *currentTank)
{
	GLState state3(GLState::TEXTURE_OFF | GLState::BLEND_ON);

	FixedVector tankPosition = currentTank->getPosition().getTankTurretPosition();
	FixedVector tankWindowPosition(25, tankPosition[2], 0);

	const fixed diff = fixed(5) / 180 * fixed::XPI;
	fixed tankElevation1 = (fixed(90) - currentTank->getPosition().getRotationGunYZ()) / 180 * fixed::XPI;
	fixed tankElevation2 = tankElevation1 + diff;
	FixedVector tankElevationDirection1(tankElevation1.sin(), tankElevation1.cos(), 0);
	FixedVector tankElevationDirection2(tankElevation2.sin(), tankElevation2.cos(), 0);

	FixedVector aimPos1 = tankWindowPosition + tankElevationDirection1  * 5;
	FixedVector aimPos2 = tankWindowPosition + tankElevationDirection1  * 30;
	FixedVector aimPos3 = tankWindowPosition + tankElevationDirection2  * 30;

	glColor3f(1.0f, 0.3f, 0.0f);

	glBegin(GL_QUADS);
		glVertex2f(aimPos2[0].asFloat(), aimPos2[1].asFloat());
		glVertex2f(aimPos1[0].asFloat(), aimPos1[1].asFloat());
		glVertex2f(aimPos1[0].asFloat(), aimPos1[1].asFloat());
		glVertex2f(aimPos3[0].asFloat(), aimPos3[1].asFloat());
	glEnd();
}

void ProfileDialog::mouseDown(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseDown(button, x, y, skipRest);
	if (x > x_ && x < x_ + w_ && y > y_ && y < y_ + h_)
	{
		zooming_ = !zooming_;

		if (profileZoom_ == 1.0f)
		{
			zoomX_ = x;
			zoomY_ = y;
		}
	}
}

void ProfileDialog::mouseUp(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseUp(button, x, y, skipRest);
	if (x > x_ && x < x_ + w_ && y > y_ && y < y_ + h_)
	{
		//zooming_ = false;
	}
}
