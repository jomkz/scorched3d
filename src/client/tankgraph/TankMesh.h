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

#if !defined(AFX_TANKMESH_H__CB857C65_A22F_4FBC_9344_EFF22F8A4EEA__INCLUDED_)
#define AFX_TANKMESH_H__CB857C65_A22F_4FBC_9344_EFF22F8A4EEA__INCLUDED_

#include <common/Vector4.h>
#include <graph/ModelRendererMesh.h>
#include <list>

class TankMesh : public ModelRendererMesh
{
public:
	TankMesh(Model &tank);
	virtual ~TankMesh();

	void draw(float frame, bool drawS, float *rotMatrix, Vector &position, 
		float fireOffSet, float rotXY, float rotXZ,
		bool absCenter = false, float scale = 1.0f, float fade = 1.0f, 
		bool setState = true);
	int getNoTris();

	static void drawSight();
protected:
	enum MeshType
	{
		eNone,
		eTurret,
		eGun
	};

	bool drawS_;
	float fireOffSet_;
	float scale_;
	float rotXY_;
	float rotXZ_;
	Vector gunOffset_;
	Vector turretCenter_;
	std::vector<MeshType> meshTypes_;

	virtual void drawMesh(unsigned int m, Mesh *mesh, float currentFrame, bool setState);
	void setupTankMesh();
};

#endif // !defined(AFX_TANKMESH_H__CB857C65_A22F_4FBC_9344_EFF22F8A4EEA__INCLUDED_)
