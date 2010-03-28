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

#include <GLW/GLWProfileView.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWPanel.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLTexture.h>
#include <image/ImageFactory.h>
#include <tank/TankContainer.h>
#include <tank/TankPosition.h>
#include <landscapemap/LandscapeMaps.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <landscape/Landscape.h>
#include <client/ScorchedClient.h>

REGISTER_CLASS_SOURCE(GLWProfileView);

GLWProfileView::GLWProfileView(float x, float y, float w, float h) :
	GLWidget(x, y, w, h)
{
}

GLWProfileView::~GLWProfileView()
{
}

void GLWProfileView::draw()
{
	static GLTexture tankTexture;
	static GLTexture arrowTexture;
	static bool createdTexture = false;
	if (!createdTexture)
	{
		createdTexture = true;

		{
			std::string file1 = S3D::getDataFile("data/images/arrow.bmp");
			std::string file2 = S3D::getDataFile("data/images/arrowi.bmp");
			ImageHandle bitmap = 
				ImageFactory::loadImageHandle(file1.c_str(), file2.c_str(), true);
			arrowTexture.create(bitmap);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		}
		{
			std::string file1 = S3D::getDataFile("data/images/tank2s.bmp");
			ImageHandle bitmap = 
				ImageFactory::loadImageHandle(file1.c_str());
			tankTexture.create(bitmap);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		}
	}


	if (parent_)
	{
		w_ = parent_->getW();
		h_ = parent_->getH();
	}

	GLWidget::draw();

	Tank *currentTank =
		ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (!currentTank || !currentTank->getAlive()) return;

	FixedVector tankPosition = currentTank->getPosition().getTankPosition();
	fixed tankRotation = -currentTank->getPosition().getRotationGunXY() / 180 * fixed::XPI;
	FixedVector tankDirection(tankRotation.sin(), tankRotation.cos(), 0);
	tankPosition -= tankDirection * 2;

	GLState state1(GLState::TEXTURE_OFF | GLState::BLEND_ON);

	glColor4f(0.13f, 0.56f, 0.91f, 0.6f);
	glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(w_, 0.0f);
		glVertex2f(w_, h_);
		glVertex2f(0.0f, h_);
	glEnd();

	glColor4f(0.0f, 0.7f, 0.0f, 1.0f);

	glBegin(GL_QUAD_STRIP);
	FixedVector position = tankPosition;
	float x = 0.0f;
	for (int i=0; i<=100; i++, x+=w_ / 100.0f)
	{
		float height =
			ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getInterpHeight(
			position[0], position[1]).asFloat() * 5.0f;
		if (height > h_) height = h_;
		else if (height < 1.0f) height = 1.0f;

		glVertex2f(x, height);
		glVertex2f(x, 0.0f);

		position += tankDirection;
	}
	glEnd();

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
			fixed dotP = (tank->getPosition().getTankPosition() - tankPosition).Normalize().dotP(tankDirection);
			if (dotP < fixed(true, 9000)) continue;
		}

		FixedVector AP = tank->getPosition().getTankPosition() - tankPosition;
		FixedVector AB = tankDirection * 100;

		fixed ab2 = AB[0]*AB[0] + AB[1]*AB[1];
		fixed ap_ab = AP[0]*AB[0] + AP[1]*AB[1];
		fixed t = ap_ab / ab2;
		if (t>=0 && t<1)
		{
			FixedVector point = tankPosition + AB * t;
			fixed landheight =
				ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getInterpHeight(
				point[0], point[1]) * 5;
			fixed x = t * fixed::fromFloat(w_);
			fixed tankheight = 
				tank->getPosition().getTankPosition()[2] * 5;

			if (landheight < tankheight + 15)
			{
				landheight = tankheight + 15;
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
						
			arrowTexture.draw();
			glColor3fv(tank->getColor());
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(x.asFloat() - 5.0f, landheight.asFloat() + 20.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(x.asFloat() - 5.0f, landheight.asFloat() + 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(x.asFloat() + 5.0f, landheight.asFloat() + 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(x.asFloat() + 5.0f, landheight.asFloat() + 20.0f);
			glEnd();

			glDepthMask(GL_FALSE);
			float textWidth =
				GLWFont::instance()->getGameFont()->getWidth(5.0f,
					tank->getTargetName());
			GLWFont::instance()->getGameFont()->draw(
				tank->getColor(), 5.0f, 
				x.asFloat() - textWidth / 2.0f, 
				landheight.asFloat() + 26.0f, 
				0.0f,
				tank->getTargetName());
			glDepthMask(GL_TRUE);

			if (tank == currentTank) tankWindowPos = FixedVector(x, tankheight, 0);
		}
	}

	GLState state3(GLState::TEXTURE_OFF | GLState::BLEND_ON);

	const fixed diff = fixed(5) / 180 * fixed::XPI;
	fixed tankElevation1 = (fixed(90) - currentTank->getPosition().getRotationGunYZ()) / 180 * fixed::XPI;
	fixed tankElevation2 = tankElevation1 + diff;
	FixedVector tankElevationDirection1(tankElevation1.sin(), tankElevation1.cos(), 0);
	FixedVector tankElevationDirection2(tankElevation2.sin(), tankElevation2.cos(), 0);
	FixedVector aimPos1 = tankWindowPos + tankElevationDirection1  * 5;
	FixedVector aimPos2 = tankWindowPos + tankElevationDirection1  * 30;
	FixedVector aimPos3 = tankWindowPos + tankElevationDirection2  * 30;

	glColor3f(1.0f, 0.3f, 0.0f);
	tankWindowPos[1] += fixed(1 * 5);

	glBegin(GL_QUADS);
		glVertex2f(aimPos2[0].asFloat(), aimPos2[1].asFloat());
		glVertex2f(aimPos1[0].asFloat(), aimPos1[1].asFloat());
		glVertex2f(aimPos1[0].asFloat(), aimPos1[1].asFloat());
		glVertex2f(aimPos3[0].asFloat(), aimPos3[1].asFloat());
	glEnd();

	glLineWidth(2.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINE_LOOP);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(w_, 0.0f);
		glVertex2f(w_, h_);
		glVertex2f(0.0f, h_);
	glEnd();
	glLineWidth(1.0f);
}
