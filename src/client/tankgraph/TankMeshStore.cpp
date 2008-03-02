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

#include <tankgraph/TankMeshStore.h>
#include <3dsparse/ModelStore.h>

TankMeshStore *TankMeshStore::instance_ = 0;

TankMeshStore *TankMeshStore::instance()
{
	if (!instance_) instance_ = new TankMeshStore();
	return instance_;
}

TankMeshStore::TankMeshStore()
{
}

TankMeshStore::~TankMeshStore()
{
}

TankMesh *TankMeshStore::getMesh(ModelID modelId)
{
	std::map<std::string, TankMesh*>::iterator findItor = 
		meshes_.find(modelId.getStringHash());

	TankMesh *mesh = 0;
	if (findItor == meshes_.end())
	{
		Model *newFile = ModelStore::instance()->loadModel(modelId);
		if (!newFile) return 0;

		// Create tank mesh
		mesh = new TankMesh(*newFile);
		meshes_[modelId.getStringHash()] = mesh;
	}
	else
	{
		mesh = (*findItor).second;
	}
	
	return mesh;
}
