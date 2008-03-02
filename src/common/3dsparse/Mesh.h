////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_Meshh_INCLUDE__)
#define __INCLUDE_Meshh_INCLUDE__

#include <common/DefinesAssert.h>
#include <common/Vector4.h>
#include <3dsparse/Face.h>
#include <3dsparse/Vertex.h>
#include <vector>
#include <string>

class Mesh
{
public:
	Mesh(const char *name);
	virtual ~Mesh();

	// Accessors
	const char *getName() { return name_.c_str(); }
	bool getSphereMap() { return sphereMap_; }
	Vector &getMin() { return min_; }
	Vector &getMax() { return max_; }
	Vector4 &getAmbientColor() { return ambientColor_; }
	Vector4 &getDiffuseColor() { return diffuseColor_; }
	Vector4 &getSpecularColor() { return specularColor_; }
	Vector4 &getEmissiveColor() { return emissiveColor_; }
	Vector4 &getAmbientNoTexColor() { return ambientNoTexColor_; }
	Vector4 &getDiffuseNoTexColor() { return diffuseNoTexColor_; }
	Vector4 &getSpecularNoTexColor() { return specularNoTexColor_; }
	Vector4 &getEmissiveNoTexColor() { return emissiveNoTexColor_; }
	float &getShininessColor() { return shininessColor_; }
	const char *getTextureName() 
		{ return textureName_.c_str(); }
	const char *getATextureName() 
		{ return aTextureName_.c_str(); }

	std::vector<int> &getCollapseMap()
		{ return collapseMap_; }
	std::vector<Face *> &getFaces() 
		{ return faces_; }
	Face *getFace(int faceIndex)
		{ DIALOG_ASSERT(faceIndex < (int) faces_.size()); 
		return faces_[faceIndex]; }
	std::vector<Vertex *> &getVertexes() 
		{ return vertexes_; }
	Vertex *getVertex(int vertexIndex)
		{ DIALOG_ASSERT(vertexIndex < (int) vertexes_.size()); 
		return vertexes_[vertexIndex]; }
	bool &getReferencesBones() { return referencesBones_; }

	// Modification
	void move(Vector &movement);

	// Creation
	void setupCollapse();
	void insertFace(Face &face) 
		{ faces_.push_back(new Face(face)); }
	void insertVertex(Vertex &vertex);
	void setTextureName(const char *t);
	void setATextureName(const char *t) { aTextureName_ = t; }
	void setFaceNormal(Vector &normal, int faceIndex, int normalIndex)
		{ DIALOG_ASSERT(faceIndex < (int) faces_.size()); 
		faces_[faceIndex]->normal[normalIndex] = normal; }
	void setFaceTCoord(Vector &coord, int faceIndex, int coordIndex)
		{ DIALOG_ASSERT(faceIndex < (int) faces_.size()); 
		faces_[faceIndex]->tcoord[coordIndex] = coord; }

protected:
	std::string name_;
	std::string textureName_, aTextureName_;
	std::vector<Face *> faces_;
	std::vector<Vertex *> vertexes_;
	std::vector<int> collapseMap_;
	bool referencesBones_;
	bool sphereMap_;
	float shininessColor_;
	Vector4 diffuseColor_, specularColor_;
	Vector4 ambientColor_, emissiveColor_;
	Vector4 diffuseNoTexColor_, specularNoTexColor_;
	Vector4 ambientNoTexColor_, emissiveNoTexColor_;
	Vector min_, max_;
};

#endif // __INCLUDE_Meshh_INCLUDE__
