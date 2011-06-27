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

#include <weapons/Weapon.h>
#include <weapons/AccessoryStore.h>

WeaponFireContextInternal::WeaponFireContextInternal() :
	killCount_(0), referenceCount_(0)
{
}

WeaponFireContextInternal::~WeaponFireContextInternal()
{
}

void WeaponFireContextInternal::incrementReference()
{
	referenceCount_++;
}

void WeaponFireContextInternal::decrementReference()
{
	referenceCount_--;
	if (referenceCount_ == 0) delete this;
}

WeaponFireContext::WeaponFireContext(unsigned int playerId, unsigned int data) :
	playerId_(playerId),
	data_(data)
{
	internalContext_ = new WeaponFireContextInternal();
	internalContext_->incrementReference();
}

WeaponFireContext::WeaponFireContext(WeaponFireContext &other) :
	playerId_(other.playerId_),
	data_(other.data_),
	labelCount_(other.labelCount_),
	internalContext_(other.internalContext_)
{
	internalContext_->incrementReference();
}

WeaponFireContext::~WeaponFireContext()
{
	internalContext_->decrementReference();
}

int WeaponFireContext::getIncLabelCount(unsigned int label)
{
	std::map<unsigned int, int>::iterator findItor =
		labelCount_.find(label);
	if (findItor == labelCount_.end()) labelCount_[label] = 0;

	return ++labelCount_[label];
}

Weapon::Weapon() : 
	armsLevel_(-1)
{

}

Weapon::~Weapon()
{

}

bool Weapon::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	// Get the optional weapon armslevel
	accessoryNode->getNamedChild("armslevel", armsLevel_, false);

	return true;
}

int Weapon::getArmsLevel()
{
	if (armsLevel_ == -1) return parent_->getArmsLevel();
	return armsLevel_;
}
