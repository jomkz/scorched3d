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

#include <scorched3dc/UIStatePlaying.h>
#include <scorched3dc/Scorched3DC.h>

class HydraxListener : public Ogre::FrameListener
{
public:
    HydraxListener(Hydrax::Hydrax *hydraX) : 
		hydraX_(hydraX)
    {
    }

    bool frameStarted(const Ogre::FrameEvent &e)
    {
        hydraX_->update(e.timeSinceLastFrame);
        return true;
    }

private:
	Hydrax::Hydrax *hydraX_;
};

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

UIStatePlaying::UIStatePlaying() : UIStateI(UIState::StatePlaying),
	sceneMgr_(0), camera_(0),
	skyX_(0)
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
}

UIStatePlaying::~UIStatePlaying()
{
}

void UIStatePlaying::createState()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating Playing Scene ***");

	Ogre::Root *ogreRoot = Scorched3DC::instance()->getOgreRoot();
	Ogre::RenderWindow *ogreRenderWindow = Scorched3DC::instance()->getOgreRenderWindow();

	// Create scene manager
	sceneMgr_ = ogreRoot->createSceneManager(Ogre::ST_GENERIC);
	sceneMgr_->setAmbientLight(Ogre::ColourValue(1, 1, 1));

	// Create the camera
	camera_ = sceneMgr_->createCamera("PlayerCam");
	camera_->setPosition(Ogre::Vector3(0, 0, 500));
	camera_->setNearClipDistance(5);
	camera_->setFarClipDistance(99999*6);
	camera_->setPosition(311.902f, 128.419f, 1539.02f);
	camera_->setDirection(0.155f, 0.1808f, -0.97f);

	// Create one viewport, entire window
	Ogre::Viewport* vp = ogreRenderWindow->addViewport(camera_);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	camera_->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// Light
	Ogre::Light *mLight0 = sceneMgr_->createLight("Light0");
	mLight0->setDiffuseColour(1, 1, 1);
	mLight0->setCastShadows(false);

	// Shadow caster
	Ogre::Light *mLight1 = sceneMgr_->createLight("Light1");
	mLight1->setType(Ogre::Light::LT_DIRECTIONAL);

	// Create HydraX
	// Create Hydrax object
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating HydraX ***");
	hydraX_ = new Hydrax::Hydrax(sceneMgr_, camera_, vp);

	// Create our projected grid module  
	Hydrax::Module::ProjectedGrid *hydraxModule = new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
		hydraX_,
		// Noise module
		new Hydrax::Noise::Perlin(/*Generic one*/),
		// Base plane
		Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
		// Normal mode
		Hydrax::MaterialManager::NM_VERTEX,
		// Projected grid options
		Hydrax::Module::ProjectedGrid::Options(/*Generic one*/));

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
	mBasicController->setMoonPhase(0.75f);
	// Add a basic cloud layer
	skyX_->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
	ogreRoot->addFrameListener(skyX_);
	ogreRenderWindow->addListener(skyX_);	

	// Add the Hydrax Rtt listener
	hydraX_->getRttManager()->addRttListener(new HydraxRttListener(skyX_, hydraX_));
	// Add frame listener
	ogreRoot->addFrameListener(new HydraxListener(hydraX_));

	updateLighting(); // Should do each time the skyx (time) changes
}

void UIStatePlaying::destroyState()
{

}

void UIStatePlaying::updateLighting()
{
	Ogre::Vector3 lightDir = skyX_->getController()->getSunDirection();

	// Calculate current color gradients point
	float point = (-lightDir.y + 1.0f) / 2.0f;
	hydraX_->setWaterColor(waterGradient_.getColor(point));

	Ogre::Vector3 sunPos = camera_->getDerivedPosition() - lightDir*skyX_->getMeshManager()->getSkydomeRadius(camera_)*0.1f;
	hydraX_->setSunPosition(sunPos);

	Ogre::Light *Light0 = sceneMgr_->getLight("Light0"),
				*Light1 = sceneMgr_->getLight("Light1");

	Light0->setPosition(camera_->getDerivedPosition() - lightDir*skyX_->getMeshManager()->getSkydomeRadius(camera_)*0.02f);
	Light1->setDirection(lightDir);

	Ogre::Vector3 sunCol = sunGradient_.getColor(point);
	Light0->setSpecularColour(sunCol.x, sunCol.y, sunCol.z);
	Ogre::Vector3 ambientCol = ambientGradient_.getColor(point);
	Light0->setDiffuseColour(ambientCol.x, ambientCol.y, ambientCol.z);
	hydraX_->setSunColor(sunCol);
}
