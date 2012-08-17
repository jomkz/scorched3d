////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <ui/BackdropDecorator.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLViewPort.h>
#include <graph/Main2DCamera.h>

BackdropDecorator::BackdropDecorator() 
{
}

BackdropDecorator::~BackdropDecorator()
{
}

bool BackdropDecorator::Initialise() 
{
	Image originalBackMap = ImageFactory::loadImage(
		S3D::eDataLocation,
		"data/images/backdrop.jpg");
	int w = originalBackMap.getWidth();
	int h = originalBackMap.getHeight();
	while (w > GLViewPort::getActualWidth() || h > GLViewPort::getActualHeight())
	{
		w /= 2;
		h /= 2;
	}

	Image backMap = originalBackMap.createResize(w, h);
	backTex_.create(backMap, false);
	return true;
}

Rocket::Core::DecoratorDataHandle BackdropDecorator::GenerateElementData(Rocket::Core::Element* element)
{
	return 0;
}

void BackdropDecorator::ReleaseElementData(Rocket::Core::DecoratorDataHandle element_data)
{

}

void BackdropDecorator::RenderElement(Rocket::Core::Element* ROCKET_UNUSED(element), Rocket::Core::DecoratorDataHandle element_data)
{
	GLState::setState(GLState::DEPTH_OFF | GLState::TEXTURE_ON);

	// Calcuate how may tiles are needed
	float wWidth = (float) GLViewPort::getWidth();
	float wHeight = (float) GLViewPort::getHeight();

	// Draw the tiled logo backdrop
	backTex_.draw(true);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, 0.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, wHeight);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(wWidth, wHeight);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(wWidth, 0.0f);
	glEnd();	
}
