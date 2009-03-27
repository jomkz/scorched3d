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

#include <GLEXT/GLViewPort.h>
#include <GLEXT/GLState.h>
#include <image/ImageFactory.h>
#include <graph/Main2DCamera.h>
#include <graph/OptionsDisplay.h>
#include <dialogs/BackdropDialog.h>
#include <common/Defines.h>

BackdropDialog *BackdropDialog::instance_ = 0;

BackdropDialog *BackdropDialog::instance()
{
	if (!instance_)
	{
		instance_ = new BackdropDialog;
	}
	return instance_;
}

BackdropDialog::BackdropDialog() : 
	GLWWindow("Backdrop", 0.0f, 0.0f, 0.0f, 0.0f, eHideName,
		"The backdrop dialog")
{
	windowLevel_ = 5000000;
}

BackdropDialog::~BackdropDialog()
{
}

void BackdropDialog::draw()
{
	drawBackground();
	drawLogo();
	drawFooter();
}

void BackdropDialog::drawBackground()
{
	static bool createdTexture = false;
	if (!createdTexture)
	{
		createdTexture = true;
		ImageHandle originalBackMap = ImageFactory::loadImageHandle(
			S3D::getDataFile("data/images/backdrop.jpg"));
		int w = originalBackMap.getWidth();
		int h = originalBackMap.getHeight();
		while (w > GLViewPort::getActualWidth() || h > GLViewPort::getActualHeight())
		{
			w /= 2;
			h /= 2;
		}

		ImageHandle backMap = originalBackMap.createResize(w, h);
		backTex_.create(backMap, false);
	}

	GLState currentState(GLState::DEPTH_OFF | GLState::TEXTURE_ON);

	// Calcuate how may tiles are needed
	float wWidth = (float) GLViewPort::getWidth();
	float wHeight = (float) GLViewPort::getHeight();

	// Draw the tiled logo backdrop
	backTex_.draw(true);
	glColor3f(1.0f, 1.0f, 1.0f);//0.2f, 0.2f, 0.2f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(wWidth, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(wWidth, wHeight);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, wHeight);
	glEnd();	
}

void BackdropDialog::drawLogo()
{
	if (S3D::getDataFileMod() != lastMod_)
	{
		lastMod_ = S3D::getDataFileMod();

		ImageHandle logoMap = ImageFactory::loadImageHandle(
			S3D::getModFile("data/windows/scorched.jpg"),
			S3D::getModFile("data/windows/scorcheda.jpg"),
			false);
		logoTex_.create(logoMap, false);
	}

	GLState currentState(GLState::DEPTH_OFF | GLState::BLEND_ON | GLState::TEXTURE_ON);

	// Calcuate how may tiles are needed
	float wWidth = (float) GLViewPort::getWidth();
	float wHeight = (float) GLViewPort::getHeight();

	// Draw the higer rez logo
	const float logoWidth = 480.0f;
	const float logoHeight = 90.0f;
	logoTex_.draw(true);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
		glTranslatef(
			wWidth - logoWidth - 50.0f, 
			wHeight - logoHeight - 50.0f, 
			0.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(logoWidth, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(logoWidth, logoHeight);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, logoHeight);
		glEnd();
	glPopMatrix();
}

void BackdropDialog::drawFooter()
{
	static bool createdTexture = false;
	if (!createdTexture)
	{
		createdTexture = true;
		ImageHandle logoMap = ImageFactory::loadAlphaImageHandle(
			S3D::getDataFile("data/images/hiscore.png"));
		footerTex_.create(logoMap, false);
	}

	GLState currentState(GLState::DEPTH_OFF | GLState::BLEND_ON | GLState::TEXTURE_ON);

	// Calcuate how may tiles are needed
	float wWidth = (float) GLViewPort::getWidth();
	float wHeight = (float) GLViewPort::getHeight();

	// Draw the higer rez logo
	const float logoWidth = 200.0f;
	const float logoHeight = 90.0f;
	footerTex_.draw(true);
	glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
	glPushMatrix();
		glTranslatef(
			wWidth - logoWidth - 10.0f,
			10.0f, 
			0.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(logoWidth, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(logoWidth, logoHeight);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, logoHeight);
		glEnd();
	glPopMatrix();
}

void BackdropDialog::capture()
{
	if (OptionsDisplay::instance()->getNoProgressBackdrop()) return;
	if (!backTex_.getTexName() == 0) return;

	glRasterPos2i(0, 0);

	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	int imageSize = GLViewPort::getActualWidth() * GLViewPort::getActualHeight() * 3;
	unsigned char *screenpixels = new unsigned char[imageSize];
	glReadPixels(0, 0, GLViewPort::getActualWidth(), GLViewPort::getActualHeight(), 
		GL_RGB, GL_UNSIGNED_BYTE, screenpixels);

	ImageHandle handle = ImageFactory::createBlank(
		backTex_.getWidth(), backTex_.getHeight());

	unsigned char *src = screenpixels;
	for (int y=0; y<GLViewPort::getActualHeight(); y++)
	{
		for (int x=0; x<GLViewPort::getActualWidth(); x++, src+=3)
		{
			int totalr = 0;
			int totalg = 0;
			int totalb = 0;
			if (x>=3 && x<GLViewPort::getActualWidth()-3 &&
				y>=3 && y<GLViewPort::getActualHeight()-3)
			{
				for (int a=-3; a<=3; a++)
				{
					for (int b=-3; b<=3; b++)
					{
						int srcx = a + x;
						int srcy = b + y;
						unsigned char *src2 = src + a * 3 + b * GLViewPort::getActualWidth() * 3;
						if (src2 >= screenpixels && src2 - screenpixels < imageSize)
						{
							totalr += src2[0];
							totalg += src2[1];
							totalb += src2[2];
						}
					}
				}
				totalr /= 49;
				totalg /= 49;
				totalb /= 49;
			}
			else
			{
				totalr = src[0];
				totalg = src[1];
				totalb = src[2];
			}

			int destx = (x * backTex_.getWidth() / GLViewPort::getActualWidth()) % backTex_.getWidth();
			int desty = (y * backTex_.getHeight() / GLViewPort::getActualHeight()) % backTex_.getHeight();
			unsigned char *dest = &handle.getBits()[destx * 3 + desty * backTex_.getWidth() * 3];

			dest[0] = totalr;
			dest[1] = totalg;
			dest[2] = totalb;
		}
	}

	delete [] screenpixels;

	backTex_.replace(handle, false);
}
