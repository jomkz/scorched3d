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

#if !defined(__INCLUDE_ModelRendererMeshh_INCLUDE__)
#define __INCLUDE_ModelRendererMeshh_INCLUDE__

#include <graph/ModelRenderer.h>
#include <3dsparse/Model.h>

class GLTexture;
class ModelRendererMesh : public ModelRenderer
{
public:
	ModelRendererMesh(Model *model);
	virtual ~ModelRendererMesh();

	virtual void draw(float currentFrame, 
		float distance, float fade, bool setState);
	virtual void drawBottomAligned(float currentFrame, 
		float distance, float fade, bool setState);

	virtual Model *getModel() { return model_; }

protected:
	struct MeshFrameInfo
	{
		MeshFrameInfo() : displayList(0), lastCachedState(0) {}

		unsigned int displayList;
		unsigned int lastCachedState;
	};

	struct MeshInfo
	{
		MeshInfo() : texture(0) {}

		GLTexture *texture;
		std::vector<MeshFrameInfo> frameInfos_;
	};

	Model *model_;
	std::vector<BoneType *> boneTypes_;
	std::vector<MeshInfo> meshInfos_;
	Vector vertexTranslation_;

	virtual void drawMesh(unsigned int m, Mesh *mesh, float currentFrame, bool setState);
	virtual void drawVerts(unsigned int m, Mesh *mesh, bool vertexLighting, int frame);
	virtual void setup();
};

#endif // __INCLUDE_ModelRendererMeshh_INCLUDE__
