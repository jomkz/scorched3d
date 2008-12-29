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

#include <graph/ModelRendererSimulator.h>
#include <graph/ModelRenderer.h>
#include <3dsparse/Model.h>
#include <stdlib.h>

ModelRendererSimulator::ModelRendererSimulator(ModelRenderer *renderer) : 
	renderer_(renderer)
{
	currentFrame_ = (float) renderer_->getModel()->getStartFrame();
	if (renderer_->getModel()->getTotalFrames() > 1)
	{
		currentFrame_ = (float) (rand() % renderer_->getModel()->getTotalFrames());		
	}
}

ModelRendererSimulator::~ModelRendererSimulator()
{
}

void ModelRendererSimulator::drawBottomAligned(float distance, float fade, bool setState)
{
	renderer_->drawBottomAligned(currentFrame_, distance, fade, setState);
}

void ModelRendererSimulator::draw(float distance, float fade, bool setState)
{
	renderer_->draw(currentFrame_, distance, fade, setState);
}

void ModelRendererSimulator::simulate(float frameTime)
{
	currentFrame_ += frameTime;
}
