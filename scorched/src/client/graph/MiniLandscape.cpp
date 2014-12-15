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

#include <graph/MiniLandscape.h>
#include <graph/LandscapeBlenderMini.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreManualObject.h>
#include <CEGUI/ImageManager.h>

MiniLandscape *MiniLandscape::instance()
{
	static MiniLandscape instance;
	return &instance;
}

MiniLandscape::MiniLandscape() : landscapeSceneManager_(0)
{
	x_size = 128;
	z_size = 128;
	half_x_size = x_size / 2.0f;
	half_z_size = z_size / 2.0f;
}

MiniLandscape::~MiniLandscape()
{
}

void MiniLandscape::create()
{
	if (landscapeSceneManager_) return;

	Ogre::Root *ogreRoot = ScorchedUI::instance()->getOgreSystem().getOgreRoot();
	landscapeSceneManager_ = ogreRoot->createSceneManager(Ogre::ST_GENERIC, "MiniLansdscapeSceneManager");

	// RTT 
	rtt_texture_ = Ogre::TextureManager::getSingleton().createManual(
		"MiniRttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		1024, 1024, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);

	// Create viewing camera
	landscapeCamera_ = landscapeSceneManager_->createCamera("MiniCamera");
	landscapeCamera_->setPosition(150, 100, 150);
	landscapeCamera_->setAspectRatio(1.0f);
	landscapeCamera_->lookAt(0, 0, 0);

	// Texture settings
	renderTexture_ = rtt_texture_->getBuffer()->getRenderTarget();
	renderTexture_->addViewport(landscapeCamera_);
	renderTexture_->getViewport(0)->setClearEveryFrame(true);
	renderTexture_->getViewport(0)->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 0));
	renderTexture_->getViewport(0)->setOverlaysEnabled(false);

	// Create a light
	Ogre::Light* pointLight = landscapeSceneManager_->createLight("MiniLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setPosition(Ogre::Vector3(0, 150, 250));
	pointLight->setDiffuseColour(1.0, 1.0, 1.0);
	pointLight->setSpecularColour(1.0, 1.0, 1.0);
	landscapeSceneManager_->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
	Ogre::SceneNode* lightNode = landscapeSceneManager_->getRootSceneNode()->createChildSceneNode("MiniLightNode");
	lightNode->attachObject(pointLight);

	// Terrain code
	miniLandscape_ = new Ogre::ManualObject("MiniLandscape");
	miniLandscape_->begin("General/MiniLandscapeLand", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	for (int i = 0; i <= x_size; i++)
	{
		for (int j = 0; j <= z_size; j++)
		{
			miniLandscape_->position(Ogre::Vector3(Ogre::Real(i) - half_x_size, 0.0, Ogre::Real(j) - half_z_size));
			miniLandscape_->textureCoord((float)i / (float)x_size, (float)j / (float)z_size);
			miniLandscape_->normal(0.0, 1.0, 0.0);
		}
	}

	for (int i = 0; i < x_size; i++)
	{
		for (int j = 0; j < z_size; j++)
		{
			miniLandscape_->quad(i * (x_size + 1) + j,
				i * (x_size + 1) + j + 1,
				(i + 1) * (x_size + 1) + j + 1,
				(i + 1) * (x_size + 1) + j);
		}
	}
	miniLandscape_->end();
	Ogre::SceneNode *landscapeNode = landscapeSceneManager_->getRootSceneNode()->createChildSceneNode("MiniLandscapeNode");
	landscapeNode->attachObject(miniLandscape_);

	// Water
	Ogre::ManualObject *miniWater = new Ogre::ManualObject("MiniWater");
	miniWater->begin("General/MiniLandscapeWater", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	miniWater->position(Ogre::Vector3(Ogre::Real(-half_x_size), 0.0, Ogre::Real(-half_z_size)));
	miniWater->normal(0.0, 1.0, 0.0);
	miniWater->textureCoord(0.0, 0.0);
	miniWater->position(Ogre::Vector3(Ogre::Real(-half_x_size), 0.0, Ogre::Real(z_size) - half_z_size));
	miniWater->normal(0.0, 1.0, 0.0);
	miniWater->textureCoord(0.0, 1.0);
	miniWater->position(Ogre::Vector3(Ogre::Real(x_size) - half_x_size, 0.0, Ogre::Real(z_size) - half_z_size));
	miniWater->normal(0.0, 1.0, 0.0);
	miniWater->textureCoord(1.0, 1.0);
	miniWater->position(Ogre::Vector3(Ogre::Real(x_size) - half_x_size, 0.0, Ogre::Real(-half_z_size)));
	miniWater->normal(0.0, 1.0, 0.0);
	miniWater->textureCoord(1.0, 0.0);
	miniWater->quad(0, 1, 2, 3);
	miniWater->end();
	waterNode_ = landscapeNode->createChildSceneNode("MiniLandscapeWaterNode");
	waterNode_->attachObject(miniWater);
	waterNode_->setPosition(0.0, 1.0, 0.0);

	Ogre::ManualObject *MiniWaterSide1 = new Ogre::ManualObject("MiniWaterSide1");
	MiniWaterSide1->begin("General/MiniLandscapeWater", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	MiniWaterSide1->position(Ogre::Vector3(-half_x_size, 0.0, -half_z_size));
	MiniWaterSide1->normal(-1.0, 0.0, 0.0);
	MiniWaterSide1->textureCoord(0.0, 0.0);
	MiniWaterSide1->position(Ogre::Vector3(-half_x_size, 0.0, half_z_size));
	MiniWaterSide1->normal(-1.0, 0.0, 0.0);
	MiniWaterSide1->textureCoord(0.0, 1.0);
	MiniWaterSide1->position(Ogre::Vector3(-half_x_size, 1.0, half_z_size));
	MiniWaterSide1->normal(-1.0, 0.0, 0.0);
	MiniWaterSide1->textureCoord(1.0, 1.0);
	MiniWaterSide1->position(Ogre::Vector3(-half_x_size, 1.0, -half_z_size));
	MiniWaterSide1->normal(-1.0, 0.0, 0.0);
	MiniWaterSide1->textureCoord(1.0, 0.0);
	MiniWaterSide1->quad(0, 1, 2, 3);
	MiniWaterSide1->end();
	Ogre::SceneNode *waterSideNode1 = landscapeNode->createChildSceneNode("MiniLandscapeWaterSideNode1");
	waterSideNode1->attachObject(MiniWaterSide1);

	Ogre::ManualObject *MiniWaterSide2 = new Ogre::ManualObject("MiniWaterSide2");
	MiniWaterSide2->begin("General/MiniLandscapeWater", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	MiniWaterSide2->position(Ogre::Vector3(half_x_size, 0.0, -half_z_size));
	MiniWaterSide2->normal(1.0, 0.0, 0.0);
	MiniWaterSide2->textureCoord(0.0, 0.0);
	MiniWaterSide2->position(Ogre::Vector3(half_x_size, 0.0, half_z_size));
	MiniWaterSide2->normal(1.0, 0.0, 0.0);
	MiniWaterSide2->textureCoord(0.0, 1.0);
	MiniWaterSide2->position(Ogre::Vector3(half_x_size, 1.0, half_z_size));
	MiniWaterSide2->normal(1.0, 0.0, 0.0);
	MiniWaterSide2->textureCoord(1.0, 1.0);
	MiniWaterSide2->position(Ogre::Vector3(half_x_size, 1.0, -half_z_size));
	MiniWaterSide2->normal(1.0, 0.0, 0.0);
	MiniWaterSide2->textureCoord(1.0, 0.0);
	MiniWaterSide2->quad(3, 2, 1, 0);
	MiniWaterSide2->end();
	Ogre::SceneNode *waterSideNode2 = landscapeNode->createChildSceneNode("MiniLandscapeWaterSideNode2");
	waterSideNode2->attachObject(MiniWaterSide2);

	Ogre::ManualObject *MiniWaterSide3 = new Ogre::ManualObject("MiniWaterSide3");
	MiniWaterSide3->begin("General/MiniLandscapeWater", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	MiniWaterSide3->position(Ogre::Vector3(half_x_size, 0.0, -half_z_size));
	MiniWaterSide3->normal(0.0, 0.0, -1.0);
	MiniWaterSide3->textureCoord(0.0, 0.0);
	MiniWaterSide3->position(Ogre::Vector3(-half_x_size, 0.0, -half_z_size));
	MiniWaterSide3->normal(0.0, 0.0, -1.0);
	MiniWaterSide3->textureCoord(0.0, 1.0);
	MiniWaterSide3->position(Ogre::Vector3(-half_x_size, 1.0, -half_z_size));
	MiniWaterSide3->normal(0.0, 0.0, -1.0);
	MiniWaterSide3->textureCoord(1.0, 1.0);
	MiniWaterSide3->position(Ogre::Vector3(half_x_size, 1.0, -half_z_size));
	MiniWaterSide3->normal(0.0, 0.0, -1.0);
	MiniWaterSide3->textureCoord(1.0, 0.0);
	MiniWaterSide3->quad(0, 1, 2, 3);
	MiniWaterSide3->end();
	Ogre::SceneNode *waterSideNode3 = landscapeNode->createChildSceneNode("MiniLandscapeWaterSideNode3");
	waterSideNode3->attachObject(MiniWaterSide3);

	Ogre::ManualObject *MiniWaterSide4 = new Ogre::ManualObject("MiniWaterSide4");
	MiniWaterSide4->begin("General/MiniLandscapeWater", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	MiniWaterSide4->position(Ogre::Vector3(half_x_size, 0.0, half_z_size));
	MiniWaterSide4->normal(0.0, 0.0, 1.0);
	MiniWaterSide4->textureCoord(0.0, 0.0);
	MiniWaterSide4->position(Ogre::Vector3(-half_x_size, 0.0, half_z_size));
	MiniWaterSide4->normal(0.0, 0.0, 1.0);
	MiniWaterSide4->textureCoord(0.0, 1.0);
	MiniWaterSide4->position(Ogre::Vector3(-half_x_size, 1.0, half_z_size));
	MiniWaterSide4->normal(0.0, 0.0, 1.0);
	MiniWaterSide4->textureCoord(1.0, 1.0);
	MiniWaterSide4->position(Ogre::Vector3(half_x_size, 1.0, half_z_size));
	MiniWaterSide4->normal(0.0, 0.0, 1.0);
	MiniWaterSide4->textureCoord(1.0, 0.0);
	MiniWaterSide4->quad(3, 2, 1, 0);
	MiniWaterSide4->end();
	Ogre::SceneNode *waterSideNode4 = landscapeNode->createChildSceneNode("MiniLandscapeWaterSideNode4");
	waterSideNode4->attachObject(MiniWaterSide4);

	// Now save the contents
	renderTexture_->update();
}

void MiniLandscape::update()
{
	HeightMap &hMap = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap();

	Ogre::Real incXPos = hMap.getMapWidth() / ((Ogre::Real) x_size);
	Ogre::Real incZPos = hMap.getMapHeight() / ((Ogre::Real) z_size);

	// Update Landscape
	miniLandscape_->beginUpdate(0);
	Ogre::Real xPos, zPos;
	int i, j;
	for (i = 0, xPos = 0.0f; i <= x_size; i++, xPos += incXPos)
	{
		for (j = 0, zPos = 0.0f; j <= z_size; j++, zPos += incZPos)
		{
			fixed fHeight = hMap.getHeight((int)xPos, (int)zPos);
			FixedVector &fNormal = hMap.getNormal((int)xPos, (int)zPos);
			Ogre::Real height = fHeight.asFloat();

			miniLandscape_->position(Ogre::Vector3(Ogre::Real(i) - half_x_size, height, Ogre::Real(j) - half_z_size));
			miniLandscape_->textureCoord((float)i / (float)x_size, (float)j / (float)z_size);
			miniLandscape_->normal(fNormal[0].asFloat(), fNormal[2].asFloat(), fNormal[1].asFloat());
		}
	}

	for (i = 0; i<x_size; i++)
	{
		for (j = 0; j<z_size; j++)
		{
			miniLandscape_->quad(i * (x_size + 1) + j,
				i * (x_size + 1) + j + 1,
				(i + 1) * (x_size + 1) + j + 1,
				(i + 1) * (x_size + 1) + j);
		}
	}
	miniLandscape_->end();

	// Water height
	LandscapeTex &tex = *ScorchedClient::instance()->getLandscapeMaps().getDescriptions().getTex();
	fixed waterHeight = tex.water.waterHeight.getValue();
	waterNode_->setPosition(0.0, waterHeight.asFloat(), 0.0);
	
	// Update material
	LandscapeBlenderMini miniBlender(*ScorchedClient::instance());
	miniBlender.calculate();
	Ogre::TexturePtr mapTexture = Ogre::TextureManager::getSingleton().createManual(
		"MiniLandTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
		miniBlender.textureMapImage.getWidth(), miniBlender.textureMapImage.getHeight(),
		0, Ogre::PF_BYTE_RGB);
	mapTexture->loadImage(miniBlender.textureMapImage);

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(
		"General/MiniLandscapeLand", // name
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->getTechnique(0)->getPass(0)->createTextureUnitState("MiniLandTex");

	// Now save the contents
	renderTexture_->update();
	//renderTexture_->writeContentsToFile("i:\\tmp\\start.png");
}

CEGUI::BasicImage *MiniLandscape::createGUITexture()
{
	CEGUI::OgreRenderer *guiRenderer_ = ScorchedUI::instance()->getOgreSystem().getGUIRenderer();

	// Retrieve CEGUI texture for the RTT
	CEGUI::Texture &rttTexture = guiRenderer_->createTexture(CEGUI::String("GECUIMiniRttTex"), rtt_texture_, false);

	// Create CEGUI Image out of it
	CEGUI::BasicImage* image = (CEGUI::BasicImage*)(&CEGUI::ImageManager::getSingleton().create("BasicImage", "RTTImage"));
	image->setTexture(&rttTexture);
	image->setArea(CEGUI::Rectf(CEGUI::Vector2f(0, 0), rttTexture.getSize()));
	image->setAutoScaled(CEGUI::ASM_Both);
	return image;
}
