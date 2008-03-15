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

#include <GLW/GLWWindView.h>
#include <client/ScorchedClient.h>
#include <graph/MainCamera.h>
#include <GLEXT/GLViewPort.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <3dsparse/ModelStore.h>
#include <graph/ModelRenderer.h>
#include <graph/ModelRendererStore.h>
#include <graph/ModelRendererSimulator.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <sky/Sky.h>
#include <math.h>

REGISTER_CLASS_SOURCE(GLWWindView);

WindDialogToolTip::WindDialogToolTip()
{
}

WindDialogToolTip::~WindDialogToolTip()
{
}

void WindDialogToolTip::populate()
{
	const char *wallTypeStr = "Currently no walls";
	OptionsTransient::WallType wallType =
		ScorchedClient::instance()->getOptionsTransient().getWallType();
	switch (wallType)
	{
	case OptionsTransient::wallBouncy:
		wallTypeStr = "Current Wall Type : Bouncy";
		break;
	case OptionsTransient::wallConcrete:
		wallTypeStr = "Current Wall Type : Concrete";
		break;
	case OptionsTransient::wallWrapAround:
		wallTypeStr = "Current Wall Type : Wrap Around";
		break;
	}

	if (ScorchedClient::instance()->
		getOptionsTransient().getWindSpeed() == 0)
	{
		setText(ToolTip::ToolTipHelp, "Wind", S3D::formatStringBuffer(
			"Displays the current wind direction\n"
			"and speed, and the wall type.\n"
			"Currently No Wind.\n"
			"%s", wallTypeStr));
	}
	else
	{
		setText(ToolTip::ToolTipHelp, "Wind", S3D::formatStringBuffer(
			"Displays the current wind direction\n"
			"and speed, and the wall type.\n"
			"Current Wind Force : %i (out of 5)\n"
			"%s",
			(int) ScorchedClient::instance()->
			getOptionsTransient().getWindSpeed().asFloat(),
			wallTypeStr));
	}
}

GLWWindView::GLWWindView(float x, float y, float w, float h) :
	GLWidget(x, y, w, h),
	listNo_(0), changeCount_(0), windModel_(0)
{
	setToolTip(new WindDialogToolTip());
}

GLWWindView::~GLWWindView()
{
	delete windModel_;
	if (glIsList(listNo_)) glDeleteLists(listNo_, 1);
}

void GLWWindView::draw()
{
	if (!windModel_)
	{
		ModelID id;
		id.initFromString("ase", "data/meshes/wind.ase", "none");
		windModel_ = new ModelRendererSimulator(
			ModelRendererStore::instance()->loadModel(id));
	}

	if (changeCount_ != Landscape::instance()->getChangeCount())
	{
		changeCount_ = Landscape::instance()->getChangeCount();
		if (glIsList(listNo_)) glDeleteLists(listNo_, 1);
		listNo_ = 0;
	}

	GLWidget::draw();
	drawDisplay();
}

void GLWWindView::drawDisplay()
{
	Vector &lookFrom = MainCamera::instance()->getCamera().getCurrentPos();
	Vector &lookAt = MainCamera::instance()->getCamera().getLookAt();
	Vector dir = (lookAt - lookFrom).Normalize();

	float scale = 0.1f;
	float scale2 = MIN(w_, h_) / 90.0f;
	float angXY = atan2f(dir[0], dir[1]) / 3.14f * 180.0f;
	float angYZ = acosf(dir[2]) / 3.14f * 180.0f + 180.0f;
	if (angYZ < 280.0f) angYZ = 280.0f;

	GLState currentState(GLState::DEPTH_ON | GLState::TEXTURE_ON);
	glPushMatrix();
		glTranslatef(x_ + w_ / 2.0f, y_ + h_ / 2.0f, 0.0f);
		glRotatef(angYZ, 1.0f, 0.0f, 0.0f);
		glRotatef(angXY, 0.0f, 0.0f, 1.0f);

		Landscape::instance()->getSky().getSun().setLightPosition(); // Reset light

		// Draw the minature landscape
		glScalef(scale2, scale2, scale2);
		if (listNo_) glCallList(listNo_);
		else
		{
			glNewList(listNo_ = glGenLists(1), GL_COMPILE_AND_EXECUTE);
				drawScene();
			glEndList();
		}

		// Draw the wind arrow for direction
		GLState texState(GLState::TEXTURE_OFF);
		if (ScorchedClient::instance()->getOptionsTransient().getWindOn())
		{
			Vector4 sunPosition(-100.0f, 100.0f, 400.0f, 1.0f);
			Vector4 sunDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
			Vector4 sunAmbient(0.4f, 0.4f, 0.4f, 1.0f);
			glLightfv(GL_LIGHT1, GL_AMBIENT, sunAmbient);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, sunDiffuse);
			glLightfv(GL_LIGHT1, GL_POSITION, sunPosition);

			glTranslatef(0.0f, 0.0f, 20.0f);
			glScalef(scale, scale, scale);

			glRotatef(-ScorchedClient::instance()->getOptionsTransient().getWindAngle().asFloat(), 
				0.0f, 0.0f, 1.0f);
			drawArrow();
		}
	glPopMatrix();

	Landscape::instance()->getSky().getSun().setLightPosition(); // Reset light
}

void GLWWindView::drawScene()
{
	Landscape::instance()->getPlanTexture().draw(true);
	glColor3f(1.0f, 1.0f, 1.0f);

	HeightMap &hmp = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap();
	int maxSize = MAX(hmp.getMapWidth(), hmp.getMapHeight());
	int sqSizeW = hmp.getMapWidth() / 16;
	int sqSizeH = hmp.getMapHeight() / 16;
	const float squareSizeWidth = 30.0f * float(hmp.getMapWidth()) / float(maxSize);
	const float squareSizeHeight = 30.0f * float(hmp.getMapHeight()) / float(maxSize);
	float heightPer = 30.0f / float(maxSize) * 2.0f;

	for (int y=0; y<=hmp.getMapHeight()-sqSizeH; y+=sqSizeH)
	{
		glBegin(GL_QUAD_STRIP);
		for (int x=0; x<=hmp.getMapWidth(); x+=sqSizeW)
		{
			float xPer = float(x) / float(hmp.getMapWidth());
			float yPer = float(y) / float(hmp.getMapHeight());
			float yPer2 = float(y+sqSizeH) / float(hmp.getMapHeight());

			float xPos = xPer * squareSizeWidth * 2 - squareSizeWidth;
			float yPos = yPer * squareSizeHeight * 2 - squareSizeHeight;
			float yPos2 = yPer2 * squareSizeHeight * 2 - squareSizeHeight;

			glTexCoord2f(xPer, yPer2);
			glVertex3f(xPos, yPos2, hmp.getHeight(x, y+sqSizeH).asFloat() * heightPer);
			glTexCoord2f(xPer, yPer);
			glVertex3f(xPos, yPos, hmp.getHeight(x, y).asFloat() * heightPer);
		}
		glEnd();
	}

	Landscape::instance()->getMagTexture().draw();
	glBegin(GL_QUAD_STRIP);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-squareSizeWidth, -squareSizeHeight, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-squareSizeWidth, -squareSizeHeight, -10.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(squareSizeWidth, -squareSizeHeight, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(squareSizeWidth, -squareSizeHeight, -10.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(squareSizeWidth, squareSizeHeight, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(squareSizeWidth, squareSizeHeight, -10.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-squareSizeWidth, squareSizeHeight, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-squareSizeWidth, squareSizeHeight, -10.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-squareSizeWidth, -squareSizeHeight, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-squareSizeWidth, -squareSizeHeight, -10.0f);
	glEnd();
}

void GLWWindView::drawArrow()
{
	windModel_->draw();
}

void GLWWindView::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		unsigned int type =
			MainCamera::instance()->getTarget().getCameraType();
		type++;
		if (type >= TargetCamera::CamFree) type = 0;

		MainCamera::instance()->getTarget().setCameraType(
			(TargetCamera::CamType) type);
	}
}
