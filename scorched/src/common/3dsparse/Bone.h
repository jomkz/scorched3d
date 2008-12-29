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

#if !defined(__INCLUDE_Boneh_INCLUDE__)
#define __INCLUDE_Boneh_INCLUDE__

#include <string>
#include <vector>
#include <common/Vector.h>

typedef float BoneMatrixType[3][4];
class BoneType
{
public:
	BoneType();

	BoneMatrixType relative_;
	BoneMatrixType absolute_;
	BoneMatrixType relativeFinal_;
	BoneMatrixType final_;
	int parent_;
};

class BonePositionKey
{
public:
	BonePositionKey(float time, Vector &pos);

	float time;
	Vector position;
};

class BoneRotationKey
{
public:
	BoneRotationKey(float time, Vector &rot);

	float time;
	Vector rotation;
};

class Bone
{
public:
	Bone(const char *name);
	virtual ~Bone();

	const char *getName() { return name_.c_str(); }
	const char *getParentName() { return parentName_.c_str(); }
	Vector &getPosition() { return position_; }
	Vector &getRotation() { return rotation_; }
	std::vector<BonePositionKey *> &getPositionKeys() 
		{ return  positionKeys_; }
	std::vector<BoneRotationKey *> &getRotationKeys()
		{ return rotationKeys_; }

	Vector &getPositionAtTime(float currentTime);
	void getRotationAtTime(float currentTime, BoneMatrixType &m);

	void addPositionKey(BonePositionKey *key) 
		{ positionKeys_.push_back(key); }
	void addRotationKey(BoneRotationKey *key) 
		{ rotationKeys_.push_back(key); }
	void setParentName(const char *parentName) 
		{ parentName_ = parentName; }
	void setPosition(Vector &pos)
		{ position_ = pos; }	
	void setRotation(Vector &rot)
		{ rotation_ = rot; }	

protected:
	std::string name_;
	std::string parentName_;
	Vector position_, rotation_;
	std::vector<BonePositionKey *> positionKeys_;
	std::vector<BoneRotationKey *> rotationKeys_;

};

#endif // __INCLUDE_Boneh_INCLUDE__
