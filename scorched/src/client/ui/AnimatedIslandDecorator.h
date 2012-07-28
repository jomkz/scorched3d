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

#if !defined(__INCLUDE_AnimatedIslandDecoratorh_INCLUDE__)
#define __INCLUDE_AnimatedIslandDecoratorh_INCLUDE__

#include <Rocket/Core/Decorator.h>
#include <common/Clock.h>
#include <GLEXT/GLTexture.h>

class AnimatedIslandDecorator : public Rocket::Core::Decorator
{
public:
	AnimatedIslandDecorator();
	virtual ~AnimatedIslandDecorator();

	bool Initialise();

	virtual Rocket::Core::DecoratorDataHandle GenerateElementData(Rocket::Core::Element* element);
	virtual void ReleaseElementData(Rocket::Core::DecoratorDataHandle element_data);
	virtual void RenderElement(Rocket::Core::Element* element, Rocket::Core::DecoratorDataHandle element_data);

protected:
	float rotation_;
	Clock clock_;
	GLTexture backTex_;

	void drawStatic();
	void drawAnimated();
	void generate();
	void simulate(float frameTime);
};

#endif
