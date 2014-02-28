////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <actions/NapalmParams.h>
#include <XML/XMLNode.h>

NapalmParams::NapalmParams() : 
	napalmTime_(8),
	napalmHeight_(2),
	stepTime_(fixed(true, 1000)),
	hurtStepTime_(2),
	hurtPerSecond_(1),
	groundScorchPer_(fixed(true, 2000)),
	effectRadius_(5),
	noSmoke_(false),
	noObjectDamage_(false),
	allowUnderWater_(false),
	luminance_(true),
	singleFlow_(true),
	noCameraTrack_(false),
	napalmTexture_("flames"),
	deformTexture_(""),
	numberParticles_(100),
	landscapeErosion_(0)
{
}

NapalmParams::~NapalmParams()
{
}

bool NapalmParams::parseXML(XMLNode *accessoryNode)
{
	// Mandatory Attributes
	if (!accessoryNode->getNamedChild("effectradius", effectRadius_)) return false;
	if (!accessoryNode->getNamedChild("napalmtexture", napalmTexture_)) return false;
	if (!accessoryNode->getNamedChild("allowunderwater", allowUnderWater_)) return false;

	accessoryNode->getNamedChild("numberparticles", numberParticles_, false);

	// Get the optional luminance node
	XMLNode *noLuminanceNode = 0; luminance_ = true;
	accessoryNode->getNamedChild("noluminance", noLuminanceNode, false);
	if (noLuminanceNode) luminance_ = false;

	XMLNode *noSingleFlowNode = 0; singleFlow_ = true;
	accessoryNode->getNamedChild("nosingleflow", noSingleFlowNode, false);
	if (noSingleFlowNode) singleFlow_ = false;

	// Optional deform texture
	if (accessoryNode->getNamedChild("deformtexture", deformTexture_, false))
	{
		if (!S3D::checkDataFile(getDeformTexture())) return false;
	}

	// Optional Attributes
	XMLNode *noSmokeNode = 0, *noObjectDamageNode = 0;
	accessoryNode->getNamedChild("groundscorchper", groundScorchPer_, false);
	accessoryNode->getNamedChild("nosmoke", noSmokeNode, false);
	accessoryNode->getNamedChild("noobjectdamage", noObjectDamageNode, false);
	accessoryNode->getNamedChild("nocameratrack", noCameraTrack_, false);
	if (noSmokeNode) noSmoke_ = true;
	if (noObjectDamageNode) noObjectDamage_ = true;

	return true;
}
