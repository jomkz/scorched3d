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

#include <uistate/UIStatePlayingEnv.h>
#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/OgreSystem.h>
#include <client/ClientOptions.h>
#include <client/ScorchedClient.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapemap/LandscapeMaps.h>

class HydraxRttListener : public Hydrax::RttManager::RttListener
{
public:
    HydraxRttListener(SkyX::SkyX *skyX, Hydrax::Hydrax *hydraX) : 
		skyX_(skyX), hydraX_(hydraX)
    {
    }

	void preRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
	{
		// If needed in any case...
		bool underwater = hydraX_->_isCurrentFrameUnderwater();

		switch (Rtt)
		{
			case Hydrax::RttManager::RTT_REFLECTION:
			{
				// No stars in the reflection map
				skyX_->setStarfieldEnabled(false);
			}
			break;

			case Hydrax::RttManager::RTT_REFRACTION:
			{
			}
			break;

			case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
			{
				// Hide SkyX components in depth maps
				skyX_->getMeshManager()->getEntity()->setVisible(false);
				skyX_->getMoonManager()->getMoonBillboard()->setVisible(false);
			}
			break;
		}
	}

	void postRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
	{
		bool underwater = hydraX_->_isCurrentFrameUnderwater();

		switch (Rtt)
		{
			case Hydrax::RttManager::RTT_REFLECTION:
			{
				skyX_->setStarfieldEnabled(true);
			}
			break;

			case Hydrax::RttManager::RTT_REFRACTION:
			{
			}
			break;

			case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
			{
				skyX_->getMeshManager()->getEntity()->setVisible(true);
				skyX_->getMoonManager()->getMoonBillboard()->setVisible(true);
			}
			break;
		}
	}

private:
	SkyX::SkyX *skyX_;
	Hydrax::Hydrax *hydraX_;
};

UIStatePlayingEnv::UIStatePlayingEnv(
	Ogre::SceneManager* sceneMgr, 
	Ogre::Camera* camera) : 
	sceneMgr_(sceneMgr), camera_(camera),
	skyX_(0), hydraX_(0)
{
	// Color gradients
	// Water
	waterGradient_ = SkyX::ColorGradient();
	waterGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209f,0.535822f,0.779105f)*0.4f, 1.0f));
	waterGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209f,0.535822f,0.729105f)*0.3f, 0.8f));
	waterGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209f,0.535822f,0.679105f)*0.25f, 0.6f));
	waterGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209f,0.535822f,0.679105f)*0.2f, 0.5f));
	waterGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209f,0.535822f,0.679105f)*0.1f, 0.45f));
	waterGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209f,0.535822f,0.679105f)*0.025f, 0.0f));
	// Sun
	sunGradient_ = SkyX::ColorGradient();
	sunGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8f,0.75f,0.55f)*1.5f, 1.0f));
	sunGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8f,0.75f,0.55f)*1.4f, 0.75f));
	sunGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8f,0.75f,0.55f)*1.3f, 0.5625f));
	sunGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.6f,0.5f,0.2f)*1.5f, 0.5f));
	sunGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5f,0.5f,0.5f)*0.25f, 0.45f));
	sunGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5f,0.5f,0.5f)*0.01f, 0.0f));
	// Ambient
	ambientGradient_ = SkyX::ColorGradient();
	ambientGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1.0f,1.0f,1.0f)*1.0f, 1.0f));
	ambientGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1.0f,1.0f,1.0f)*1.0f, 0.6f));
	ambientGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1.0f,1.0f,1.0f)*0.6f, 0.5f));
	ambientGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1.0f,1.0f,1.0f)*0.3f, 0.45f));
	ambientGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1.0f,1.0f,1.0f)*0.1f, 0.35f));
	ambientGradient_.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1.0f,1.0f,1.0f)*0.05f, 0.0f));

	create();
}

UIStatePlayingEnv::~UIStatePlayingEnv()
{
}

void UIStatePlayingEnv::create()
{
	LandscapeTex *tex = ScorchedClient::instance()->getLandscapeMaps().getDescriptions().getTex();
	Ogre::Root *ogreRoot = ScorchedUI::instance()->getOgreSystem().getOgreRoot();
	Ogre::RenderWindow *ogreRenderWindow = ScorchedUI::instance()->getOgreSystem().getOgreRenderWindow();

	sceneMgr_->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));

	// Light
	sunLight_ = sceneMgr_->createLight("LightSun");
	sunLight_->setDiffuseColour(1, 1, 1); // Temp this is set properly later
	sunLight_->setCastShadows(false);

	// Shadow caster
	shadowLight_ = sceneMgr_->createLight("LightShadow");
    shadowLight_->setType(Ogre::Light::LT_DIRECTIONAL);
    shadowLight_->setDiffuseColour(Ogre::ColourValue::White);
    shadowLight_->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	// Create HydraX
	// Create Hydrax object
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating HydraX ***");
	hydraX_ = new Hydrax::Hydrax(sceneMgr_, camera_, camera_->getViewport());

	// Water grid options
	Hydrax::Module::ProjectedGrid::Options projectedOptions; /*Generic one*/
	projectedOptions.Strength = tex->water.strength.getValue().asFloat();
	projectedOptions.ChoppyStrength = tex->water.choppystrength.getValue().asFloat();
	projectedOptions.ChoppyWaves = tex->water.choppywaves.getValue();
	projectedOptions.Smooth = tex->water.smooth.getValue();
	projectedOptions.Elevation = tex->water.waterHeight.getValue().asFloat() * OgreSystem::OGRE_WORLD_HEIGHT_SCALE;

	// Create our projected grid module  
	Hydrax::Module::ProjectedGrid *hydraxModule = new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
		hydraX_,
		// Noise module
		new Hydrax::Noise::Perlin(true),
		// Base plane
		Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
		// Normal mode
		Hydrax::MaterialManager::NM_VERTEX,
		// Projected grid options
		projectedOptions);

	// Set our module
	hydraX_->setModule(static_cast<Hydrax::Module::Module*>(hydraxModule));

	// Load all parameters from config file
	// Remarks: The config file must be in Hydrax resource group.
	// All parameters can be set/updated directly by code(Like previous versions),
	// but due to the high number of customizable parameters, since 0.4 version, Hydrax allows save/load config files.
	hydraX_->loadCfg("HydraxDemo.hdx");
	hydraX_->create();

	// Create SkyX
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating SkyX ***");
	SkyX::BasicController *mBasicController = new SkyX::BasicController();
	skyX_ = new SkyX::SkyX(sceneMgr_, mBasicController);
	skyX_->create();
	skyX_->setTimeMultiplier(0.0f); // Pause simulation
	mBasicController->setTime(Ogre::Vector3(5, 4, 20));
	mBasicController->setMoonPhase(0.75f);
	// Add a basic cloud layer
	skyX_->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
	ogreRoot->addFrameListener(skyX_);
	ogreRenderWindow->addListener(skyX_);	

	// Add the Hydrax Rtt listener
	hydraX_->getRttManager()->addRttListener(new HydraxRttListener(skyX_, hydraX_));

	updateLighting(); // Should do each time the skyx (time) changes
}

void UIStatePlayingEnv::updateLighting()
{
	Ogre::Vector3 lightDir = skyX_->getController()->getSunDirection();

	// Calculate current color gradients point
	float point = (-lightDir.y + 1.0f) / 2.0f;
	hydraX_->setWaterColor(waterGradient_.getColor(point));

	Ogre::Vector3 sunPos = camera_->getDerivedPosition() - lightDir*skyX_->getMeshManager()->getSkydomeRadius(camera_)*0.1f;
	hydraX_->setSunPosition(sunPos);

	sunLight_->setPosition(camera_->getDerivedPosition() - lightDir*skyX_->getMeshManager()->getSkydomeRadius(camera_)*0.02f);
	shadowLight_->setDirection(-lightDir);

	Ogre::Vector3 sunCol = sunGradient_.getColor(point);
	sunLight_->setSpecularColour(sunCol.x, sunCol.y, sunCol.z);
	Ogre::Vector3 ambientCol = ambientGradient_.getColor(point);
	sunLight_->setDiffuseColour(ambientCol.x, ambientCol.y, ambientCol.z);
	hydraX_->setSunColor(sunCol);
}

void UIStatePlayingEnv::update(float frameTime)
{
	//updateLighting(); // Don't need to do this at the moment as the sky simulation is paused
	if (ClientOptions::instance()->getWaterAnimate())
	{
		hydraX_->update(frameTime);
	}
	if (ClientOptions::instance()->getWaterDraw() != hydraX_->isVisible())
	{
		hydraX_->setVisible(ClientOptions::instance()->getWaterDraw());
	}
	Ogre::PolygonMode waterPolyMode = 
		ClientOptions::instance()->getWaterWireframe()?Ogre::PM_WIREFRAME:Ogre::PM_SOLID;
	if (waterPolyMode != hydraX_->getPolygonMode())
	{
		hydraX_->setPolygonMode(waterPolyMode);
	}

	if (ClientOptions::instance()->getSkyDraw() != skyX_->isVisible())
	{
		skyX_->setVisible(ClientOptions::instance()->getSkyDraw());
	}
}
