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

#include <graph/ModelRendererMesh.h>
#include <graph/OptionsDisplay.h>
#include <graph/TextureStore.h>
#include <3dsparse/ModelMaths.h>
#include <GLEXT/GLGlobalState.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLInfo.h>

ModelRendererMesh::ModelRendererMesh(Model *model) : 
	model_(model)
{
	setup();
}

ModelRendererMesh::~ModelRendererMesh()
{
	while (!boneTypes_.empty())
	{
		BoneType *type = boneTypes_.back();
		boneTypes_.pop_back();
		delete type;
	}
	while (!meshInfos_.empty())
	{
		MeshInfo info = meshInfos_.back();
		meshInfos_.pop_back();

		while (!info.frameInfos_.empty())
		{
			MeshFrameInfo frameInfo = info.frameInfos_.back();
			info.frameInfos_.pop_back();
			if (frameInfo.displayList != 0)
			{
				glDeleteLists(frameInfo.displayList, 1);
			}
		}
	}
}

void ModelRendererMesh::setup()
{
	std::vector<BoneType *> &baseTypes = model_->getBaseBoneTypes();
	std::vector<BoneType *>::iterator itor;
	for (itor = baseTypes.begin();
		itor != baseTypes.end();
		itor++)
	{
		boneTypes_.push_back(new BoneType(*(*itor)));
	}

	MeshInfo info;
	MeshFrameInfo frameInfo;
	for (int f=0; f<=model_->getTotalFrames(); f++)
	{
		info.frameInfos_.push_back(frameInfo);
	}
	
	for (unsigned int m=0; m<model_->getMeshes().size(); m++)
	{
		meshInfos_.push_back(info);
	}
}

void ModelRendererMesh::drawBottomAligned(float currentFrame, 
	float distance, float fade, bool setState)
{
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -model_->getMin()[2].asFloat());
		draw(currentFrame, distance, fade, setState);
	glPopMatrix();
}

void ModelRendererMesh::draw(float currentFrame, 
	float distance, float fade, bool setState)
{
	GLGlobalState globalState(GLState::BLEND_ON | GLState::ALPHATEST_ON);

	// Set transparency on
	// Fade the model (make it transparent)
	bool useBlendColor = (GLStateExtension::hasBlendColor() && fade < 1.0f);
	if (useBlendColor)
	{
		fade = MIN(1.0f, MAX(fade, 0.2f));
		glBlendColorEXT(0.0f, 0.0f, 0.0f, fade);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA_EXT);
	}

	// Draw the model
	for (unsigned int m=0; m<model_->getMeshes().size(); m++)
	{
		Mesh *mesh = model_->getMeshes()[m];
		drawMesh(m, mesh, currentFrame, setState);
	}

	// Turn off fading
	if (useBlendColor)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void ModelRendererMesh::drawMesh(unsigned int m, Mesh *mesh, float currentFrame, bool setState)
{
	MeshInfo &meshInfo = meshInfos_[m];

	bool vertexLighting = OptionsDisplay::instance()->getNoModelLighting();
	bool useTextures =
		(!OptionsDisplay::instance()->getNoSkins() &&
		mesh->getTextureName()[0]);
	unsigned state = 0;
	
	//if (setState)
	{
		if (useTextures)
		{
			state |= GLState::TEXTURE_ON;
			if (!meshInfo.texture)
			{
				meshInfo.texture = 
					TextureStore::instance()->loadTexture(
 						mesh->getTextureName(), mesh->getATextureName());
			}
			if (meshInfo.texture) meshInfo.texture->draw();
			
			if (mesh->getSphereMap())
			{
				state |= GLState::NORMALIZE_ON;

				glEnable(GL_TEXTURE_GEN_S);						
				glEnable(GL_TEXTURE_GEN_T);	
				glEnable(GL_TEXTURE_GEN_R);
				glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				glTexGenf(GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			}
		}
		else
		{
			state |= GLState::TEXTURE_OFF;
		}

		if (!vertexLighting)
		{
			state |= 
				GLState::NORMALIZE_ON | 
				GLState::LIGHTING_ON | 
				GLState::LIGHT1_ON;

			if (useTextures)
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mesh->getAmbientColor().asVector4());
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mesh->getDiffuseColor().asVector4());
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mesh->getSpecularColor().asVector4());
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh->getEmissiveColor().asVector4());
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mesh->getShininessColor().asFloat());
			}
			else
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mesh->getAmbientNoTexColor().asVector4());
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mesh->getDiffuseNoTexColor().asVector4());
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mesh->getSpecularNoTexColor().asVector4());
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh->getEmissiveNoTexColor().asVector4());
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mesh->getShininessColor().asFloat());
			}
		}
		else
		{
			state |= 
				GLState::NORMALIZE_OFF | 
				GLState::LIGHTING_OFF | 
				GLState::LIGHT1_OFF;
		}
	}

	// Get the current frame for the animation
	// If we have no bones, when we only have one frame
	int frame = model_->getStartFrame();
	if (mesh->getReferencesBones())
	{
		// If we have bones, make sure the frame falls within the accepted bounds
		if (model_->getTotalFrames() > 1)
		{
			frame = ((unsigned int)(currentFrame)) % model_->getTotalFrames();
			if (frame < 0) frame = 0;
		}
	}

	GLGlobalState globalState(state);

	{
		int frameNo = frame;
		DIALOG_ASSERT(frameNo >= 0 && frameNo < (int) meshInfo.frameInfos_.size());
		
		unsigned int lastState = meshInfo.frameInfos_[frameNo].lastCachedState;
		unsigned int displayList = meshInfo.frameInfos_[frameNo].displayList;
		if (lastState != state)
		{
			if (displayList != 0)
			{
				glDeleteLists(displayList, 1);
				displayList = 0;
			}
			meshInfo.frameInfos_[frameNo].lastCachedState = state;
		}

		if (!displayList)
		{
			glNewList(displayList = glGenLists(1), GL_COMPILE);
				drawVerts(m, mesh, vertexLighting, frame);
			glEndList();

			meshInfo.frameInfos_[frameNo].displayList = displayList;
		}

		glCallList(displayList);
		GLInfo::addNoTriangles((int) mesh->getFaces().size());
	}

	if (useTextures)
	{
		if (mesh->getSphereMap())
		{
			glDisable(GL_TEXTURE_GEN_S);						
			glDisable(GL_TEXTURE_GEN_T);	
			glDisable(GL_TEXTURE_GEN_R);
		}
	}
}

void ModelRendererMesh::drawVerts(unsigned int m, Mesh *mesh, bool vertexLighting, int frame)
{
	// Move the bones into position
	for (unsigned int b=0; b<boneTypes_.size(); b++)
	{
		Bone *bone = model_->getBones()[b];
		BoneType *type = boneTypes_[b];

		unsigned int posKeys = (unsigned int) bone->getPositionKeys().size();
		unsigned int rotKeys = (unsigned int) bone->getRotationKeys().size();
		if (posKeys == 0 && rotKeys == 0)
		{
			memcpy(type->final_, type->absolute_, sizeof(BoneMatrixType));
			continue;
		}

		BoneMatrixType m;
		bone->getRotationAtTime(fixed(frame), m);
				
		FixedVector &pos = bone->getPositionAtTime(fixed(frame));
		m[0][3] = pos[0];
		m[1][3] = pos[1];
		m[2][3] = pos[2];

		ModelMaths::concatTransforms(type->relative_, m, type->relativeFinal_);
		if (type->parent_ == -1)
		{
			memcpy(type->final_, type->relativeFinal_, sizeof(BoneMatrixType));
		}
		else
		{
			BoneType *parent = boneTypes_[type->parent_];
			ModelMaths::concatTransforms(parent->final_, type->relativeFinal_, type->final_);
		}
	}

	// Draw the vertices
	FixedVector vec;
	glBegin(GL_TRIANGLES);

	int faceVerts[3];
	
	std::vector<Face *>::iterator itor;
	for (itor = mesh->getFaces().begin();
		itor != mesh->getFaces().end();
		itor++)
	{
		Face *face = *itor;

		for (int i=0; i<3; i++)
		{
			int index = face->v[i];
			faceVerts[i] = index;
		}
		
		if (faceVerts[0] != faceVerts[1] &&
			faceVerts[1] != faceVerts[2] &&
			faceVerts[0] != faceVerts[2])
		{
			GLInfo::addNoTriangles(1);
			for (int i=0; i<3; i++)
			{
				Vertex *vertex = mesh->getVertex(faceVerts[i]);

				if (vertexLighting)
				{
					if (GLState::getState() & GLState::TEXTURE_OFF) 
					{
						glColor3f(
							(mesh->getDiffuseNoTexColor()[0] * vertex->lightintense[0]).asFloat(),
							(mesh->getDiffuseNoTexColor()[1] * vertex->lightintense[1]).asFloat(),
							(mesh->getDiffuseNoTexColor()[2] * vertex->lightintense[2]).asFloat());
					}
					else
					{
						glColor3fv(vertex->lightintense.asVector());
					}
				}

				glTexCoord2f(face->tcoord[i][0].asFloat(), face->tcoord[i][1].asFloat());
				glNormal3fv(face->normal[i].asVector());

				if (vertex->boneIndex != -1)
				{
					BoneType *type = boneTypes_[vertex->boneIndex];

					// Note: Translation of MS to S3D coords
					FixedVector newPos, newVec;
					newPos[0] = vertex->position[0];
					newPos[1] = vertex->position[2];
					newPos[2] = vertex->position[1];

					ModelMaths::vectorRotate(newPos, type->final_, newVec);
					vec[0] = newVec[0];
					vec[1] = newVec[2];
					vec[2] = newVec[1];

					vec[0] += type->final_[0][3] + vertexTranslation_[0];
					vec[1] += type->final_[2][3] + vertexTranslation_[1];
					vec[2] += type->final_[1][3] + vertexTranslation_[2];
					
				}
				else
				{
					vec[0] = vertex->position[0] + vertexTranslation_[0];
					vec[1] = vertex->position[1] + vertexTranslation_[1];
					vec[2] = vertex->position[2] + vertexTranslation_[2];
				}

				glVertex3fv(vec.asVector());
			}
		}
	}
	glEnd();
}
