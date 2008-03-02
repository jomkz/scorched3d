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

#include <weapons/WeaponExplosion.h>
#include <weapons/Accessory.h>
#include <engine/ActionController.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponExplosion);

WeaponExplosion::WeaponExplosion() : size_(0), 
	multiColored_(false), hurtAmount_(0),
	deform_(Explosion::DeformNone),
	createDebris_(true), createMushroomAmount_(0),
	createSplash_(true), windAffected_(true),
	luminance_(true), animate_(true),
	onlyHurtShield_(false), explodeUnderGround_(true),
	minLife_(true, 5000), maxLife_(1), shake_(0),
	explosionTexture_("exp00")
{

}

WeaponExplosion::~WeaponExplosion()
{

}

bool WeaponExplosion::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

    // Get the accessory size
    if (!accessoryNode->getNamedChild("size", sizeExp_)) return false;

    // Get the accessory colored
    XMLNode *colorNode = 0;
	accessoryNode->getNamedChild("multicolor", colorNode, false);
    if (colorNode) multiColored_ = true;

    // Get the hutiness
	if (!accessoryNode->getNamedChild("hurtamount", hurtAmountExp_)) return false;

	// Get the weapon model explosion shake
	accessoryNode->getNamedChild("explosionshake", shakeExp_, false);

	// Get the hurt shield node
	accessoryNode->getNamedChild("onlyhurtshield", onlyHurtShield_, false);

	accessoryNode->getNamedChild("explodeunderground", explodeUnderGround_, false);

	// Get the no debris node
	XMLNode *noCreateDebrisNode = 0;
	accessoryNode->getNamedChild("nocreatedebris", noCreateDebrisNode, false);
	if (noCreateDebrisNode) createDebris_ = false;

	// Get the no splash node
	XMLNode *noCreateSplashNode = 0;
	accessoryNode->getNamedChild("nocreatesplash", noCreateSplashNode, false);
	if (noCreateSplashNode) createSplash_ = false;

	// Get the createmushroomamount node
	accessoryNode->getNamedChild("createmushroomamount", createMushroomAmountExp_, false);

	// Get the no windaffecting
	XMLNode *noWindAffectedNode = 0;
	accessoryNode->getNamedChild("nowindaffected", noWindAffectedNode, false);
	if (noWindAffectedNode) windAffected_ = false;

	// Get the no luminance node
	XMLNode *noLuminanceNode = 0;
	accessoryNode->getNamedChild("noluminance", noLuminanceNode, false);
	if (noLuminanceNode) luminance_ = false;

	// Get the animate node
	XMLNode *animateNode = 0;
	accessoryNode->getNamedChild("animate", animateNode, false);
	if (animateNode) animate_ = true;
	XMLNode *noAnimateNode = 0;
	accessoryNode->getNamedChild("noanimate", noAnimateNode, false);
	if (noAnimateNode) animate_ = false;

	// Get the optional explosion life nodes
	accessoryNode->getNamedChild("minlife", minLifeExp_, false);
	accessoryNode->getNamedChild("maxlife", maxLifeExp_, false);

	// Get tje deform texture
	if (accessoryNode->getNamedChild("deformtexture", deformTexture_, false))
	{
		if (!S3D::checkDataFile(getDeformTexture())) return false;
	}

	// Get the explosion texture
	accessoryNode->getNamedChild("explosiontexture", explosionTexture_, false);

	// Get the explosion sound
	if (!accessoryNode->getNamedChild("explosionsound", explosionSound_, false))
	{
		if (!S3D::checkDataFile(S3D::formatStringBuffer("data/wav/%s", getExplosionSound()))) return false;
	}

	// Get the deform
	XMLNode *deformNode = 0;
	if (!accessoryNode->getNamedChild("deform", deformNode)) return false;
    if (0 == strcmp(deformNode->getContent(), "down")) deform_ = Explosion::DeformDown;
	else if (0 == strcmp(deformNode->getContent(), "up")) deform_ = Explosion::DeformUp;
	else if (0 == strcmp(deformNode->getContent(), "none")) deform_ = Explosion::DeformNone;
	else
	{
        S3D::dialogMessage("Accessory", S3D::formatStringBuffer(
            "Unknown deform type \"%s\" in accessory \"%s\" should be up, down or none",
            deformNode->getContent(), parent_->getName()));
        return false;		
	}

	return true;
}

const char *WeaponExplosion::getExplosionTexture()
{
	return explosionTexture_.c_str();
}

const char *WeaponExplosion::getExplosionSound()
{
	if (!explosionSound_.c_str()[0]) return 0;
	return explosionSound_.c_str();
}

FixedVector &WeaponExplosion::getExplosionColor()
{
	static FixedVector white(1, 1, 1);
	if (!multiColored_) return white;

    static FixedVector red(1, 0, 0);
    static FixedVector green(0, 1, 0);
    static FixedVector blue(0, 0, 1);
    static FixedVector yellow(1, 1, 0);

    int color = int(RAND * 4.0f);
    switch (color)
    {
    case 0:
        return red;
    case 1:
        return green;
    case 2:
        return blue;
    case 3:
        return yellow;
    }
    return white;
}

void WeaponExplosion::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	size_ = sizeExp_.getValue(context, size_);
	shake_= shakeExp_.getValue(context, shake_);
	minLife_ = minLifeExp_.getValue(context, minLife_);
	maxLife_ = maxLifeExp_.getValue(context, maxLife_);
	createMushroomAmount_ = createMushroomAmountExp_.getValue(context, createMushroomAmount_);
	hurtAmount_ = hurtAmountExp_.getValue(context, hurtAmount_);

	Action *action = new Explosion(
		position, this, weaponContext);
	context.actionController->addAction(action);	
}

