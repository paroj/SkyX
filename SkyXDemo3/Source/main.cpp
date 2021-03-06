/// SkyX 0.4 Demo 3: Multi-camera

// ---------------------------------------------------------------------------
// Include SkyX header files
// ---------------------------------------------------------------------------
#include <SkyX.h>

// ----------------------------------------------------------------------------
// Include the main OGRE header files
// Ogre.h just expands to including lots of individual OGRE header files
// ----------------------------------------------------------------------------
#include <Ogre.h>
#include <Overlay/OgreTextAreaOverlayElement.h>
#include <OgreOverlay.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

// ----------------------------------------------------------------------------
// Include the OGRE example framework
// This includes the classes defined to make getting an OGRE application running
// a lot easier. It automatically sets up all the main objects and allows you to
// just override the bits you want to instead of writing it all from scratch.
// ----------------------------------------------------------------------------
#include <ExampleApplication.h>
#include <iostream>

// ----------------------------------------------------------------------------
// SkyX-related ptrs
// ----------------------------------------------------------------------------
SkyX::SkyX* mSkyX = 0;
SkyX::BasicController* mBasicController = 0;

// ----------------------------------------------------------------------------
// Misc
// ----------------------------------------------------------------------------
Ogre::TextAreaOverlayElement* mTextArea = 0;
Ogre::uint32 mCurrentPreset = 0;
Ogre::Camera* mVirtualCamera = 0;
Ogre::Camera* mCameras[2];
Ogre::uint32 mSelectedCamera = 0;

Ogre::String buildInfoStr()
{
	Ogre::String str = "SkyX 0.4 demo - Select preset: [1] Desert, [2] Clear, [3] Thunderstorm (1), [4] Thunderstorm (2), [5] Sunset, [6] Night\nCurrent preset: ";

	switch (mCurrentPreset)
	{
		case 0: {str += "1 - Sunset";} break;
		case 1: {str += "2 - Clear";} break;
		case 2: {str += "3 - Thunderstorm (1)";} break;
		case 3: {str += "4 - Thunderstorm (2)";} break;
		case 4: {str += "5 - Desert";} break;
		case 5: {str += "6 - Night";} break;
	}

	str += "\nSelect camera: [l] Left, [r] Right, Current camera: ";
	str += mSelectedCamera ? "right" : "left";

	return str;
}

void setPreset(const SkyXSettings& preset)
{
	mSkyX->setTimeMultiplier(preset.timeMultiplier);
	mBasicController->setTime(preset.time);
	mBasicController->setMoonPhase(preset.moonPhase);
	mSkyX->getAtmosphereManager()->setOptions(preset.atmosphereOpt);

	// Layered clouds
	if (preset.layeredClouds)
	{
		// Create layer cloud
		if (mSkyX->getCloudsManager()->getCloudLayers().empty())
		{
			mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
		}
	}
	else
	{
		// Remove layer cloud
		if (!mSkyX->getCloudsManager()->getCloudLayers().empty())
		{
			mSkyX->getCloudsManager()->removeAll();
		}
	}

	mSkyX->getVCloudsManager()->setWindSpeed(preset.vcWindSpeed);
	mSkyX->getVCloudsManager()->setAutoupdate(preset.vcAutoupdate);

	SkyX::VClouds::VClouds* vclouds = mSkyX->getVCloudsManager()->getVClouds();

	vclouds->setWindDirection(preset.vcWindDir);
	vclouds->setAmbientColor(preset.vcAmbientColor);
	vclouds->setLightResponse(preset.vcLightResponse);
	vclouds->setAmbientFactors(preset.vcAmbientFactors);
	vclouds->setWheater(preset.vcWheater.x, preset.vcWheater.y, false);

	if (preset.volumetricClouds)
	{
		// Create VClouds
		if (!mSkyX->getVCloudsManager()->isCreated())
		{
			// SkyX::MeshManager::getSkydomeRadius(...) works for both finite and infinite(=0) camera far clip distances
			mSkyX->getVCloudsManager()->create(mSkyX->getMeshManager()->getSkydomeRadius(mCameras[0]));
		}
	}
	else
	{
		// Remove VClouds
		if (mSkyX->getVCloudsManager()->isCreated())
		{
			mSkyX->getVCloudsManager()->remove();
		}
	}

	vclouds->getLightningManager()->setEnabled(preset.vcLightnings);
	vclouds->getLightningManager()->setAverageLightningApparitionTime(preset.vcLightningsAT);
	vclouds->getLightningManager()->setLightningColor(preset.vcLightningsColor);
	vclouds->getLightningManager()->setLightningTimeMultiplier(preset.vcLightningsTM);

	mTextArea->setCaption(buildInfoStr());

	mSkyX->update(0);
}

// ----------------------------------------------------------------------------
// Define the application object
// This is derived from ExampleApplication which is the class OGRE provides to
// make it easier to set up OGRE without rewriting the same code all the time.
// You can override extra methods of ExampleApplication if you want to further
// specialise the setup routine, otherwise the only mandatory override is the
// 'createScene' method which is where you set up your own personal scene.
// ----------------------------------------------------------------------------
#if 0
class SkyXDemoFrameListener : public ExampleFrameListener
{
public:
    SceneManager *mSceneMgr;
    Real mKeyBuffer;

    SkyXDemoFrameListener(RenderWindow* win, Camera* cam, SceneManager *sm)
            : ExampleFrameListener(win,cam)
            , mSceneMgr(sm)
            , mKeyBuffer(-1)
    {
    }

    bool frameStarted(const FrameEvent &e)
    {
        mKeyboard->capture();

		// Clamp camera(virtual camera) height
		if (mCamera->getDerivedPosition().y < -400)
		{
			mCamera->setPosition(Ogre::Vector3(mCamera->getDerivedPosition().x, -400, mCamera->getDerivedPosition().z));
		}
		else if (mCamera->getDerivedPosition().y > 800)
		{
			mCamera->setPosition(Ogre::Vector3(mCamera->getDerivedPosition().x, 800, mCamera->getDerivedPosition().z));
		}

		// Update the selected camera position/orientation
		mCameras[mSelectedCamera]->setPosition(mVirtualCamera->getPosition());
		mCameras[mSelectedCamera]->setOrientation(mVirtualCamera->getOrientation());

		// Switch presets
		if (mKeyboard->isKeyDown(OIS::KC_1) && mKeyBuffer < 0)
		{
			mCurrentPreset = 0;
			setPreset(mPresets[mCurrentPreset]);

			mKeyBuffer = 0.25f;
		}
		else if (mKeyboard->isKeyDown(OIS::KC_2) && mKeyBuffer < 0)
		{
			mCurrentPreset = 1;
			setPreset(mPresets[mCurrentPreset]);

			mKeyBuffer = 0.25f;
		}
		else if (mKeyboard->isKeyDown(OIS::KC_3) && mKeyBuffer < 0)
		{
			mCurrentPreset = 2;
			setPreset(mPresets[mCurrentPreset]);

			mKeyBuffer = 0.25f;
		}
		else if (mKeyboard->isKeyDown(OIS::KC_4) && mKeyBuffer < 0)
		{
			mCurrentPreset = 3;
			setPreset(mPresets[mCurrentPreset]);

			mKeyBuffer = 0.25f;
		}
		else if (mKeyboard->isKeyDown(OIS::KC_5) && mKeyBuffer < 0)
		{
			mCurrentPreset = 4;
			setPreset(mPresets[mCurrentPreset]);

			mKeyBuffer = 0.25f;
		}
		else if (mKeyboard->isKeyDown(OIS::KC_6) && mKeyBuffer < 0)
		{
			mCurrentPreset = 5;
			setPreset(mPresets[mCurrentPreset]);

			mKeyBuffer = 0.25f;
		}
		else if (mKeyboard->isKeyDown(OIS::KC_L) && mKeyBuffer < 0)
		{
			mSelectedCamera = 0;
			mVirtualCamera->setPosition(mCameras[mSelectedCamera]->getPosition());
			mVirtualCamera->setOrientation(mCameras[mSelectedCamera]->getOrientation());

			mKeyBuffer = 0.25f;
		}
		else if (mKeyboard->isKeyDown(OIS::KC_R) && mKeyBuffer < 0)
		{
			mSelectedCamera = 1;
			mVirtualCamera->setPosition(mCameras[mSelectedCamera]->getPosition());
			mVirtualCamera->setOrientation(mCameras[mSelectedCamera]->getOrientation());

			mKeyBuffer = 0.25f;
		}

		mKeyBuffer -= e.timeSinceLastFrame;

        return true;
    }
};
#endif

class SampleApp : public ExampleApplication
{
public:
    // Basic constructor
    SampleApp()
    {}

protected:

    // Just override the mandatory create scene method
    void createScene(void)
    {
		// Create our two rendering cameras. Since we're using the ExampleApplication and we want to individually translate/rotate
		// our two cameras, I'll use the ExampleApplication::mCamera as a virtual camera(it will not render anything) and use it
		// to translate our selected camera.
		mVirtualCamera = mCamera;

		mCameras[0] = mSceneMgr->createCamera("Camera1");
		mCameras[1] = mSceneMgr->createCamera("Camera2");

		for(Ogre::uint32 k = 0; k < 2; k++)
		{
			mCameras[k]->setPosition(mVirtualCamera->getPosition());
			mCameras[k]->setOrientation(mVirtualCamera->getOrientation());
			mCameras[k]->setAspectRatio(mVirtualCamera->getAspectRatio()/2); // /2 since we're going to use a half-width viewport size
			mCameras[k]->setFarClipDistance(30000);
			mCameras[k]->setNearClipDistance(20);
		}

		// Remove the existing viewport (created by the ExampleApplication) and create our new viewports
		mWindow->removeViewport(0);
		
		Ogre::Viewport* vp1 = mWindow->addViewport(mCameras[0], 0, 0, 0, 0.5, 1);
		Ogre::Viewport* vp2 = mWindow->addViewport(mCameras[1], 1, 0.5, 0, 0.5, 1);
		vp2->setOverlaysEnabled(false);

		// Create our text area for display SkyX parameters
		createTextArea();

		// Create SkyX
		mBasicController = new SkyX::BasicController();
		mSkyX = new SkyX::SkyX(mSceneMgr, mBasicController);
		mSkyX->create();

		// Distance geometry falling is a feature introduced in SkyX 0.2
		// When distance falling is enabled, the geometry linearly falls with the distance and the
		// amount of falling in world units is determinated by the distance between the cloud field "plane"
		// and the camera height multiplied by the falling factor.
		// For this demo, a falling factor of two is good enough for the point of view we're using. That means that if the camera
		// is at a distance of 100 world units from the cloud field, the fartest geometry will fall 2*100 = 200 world units.
		// This way the cloud field covers a big part of the sky even if the camera is in at a very low altitude.
		// The second parameter is the max amount of falling distance in world units. That's needed when for example, you've an 
		// ocean and you don't want to have the volumetric cloud field geometry falling into the water when the camera is underwater.
		// -1 means that there's not falling limit.
		mSkyX->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(2,-1));

		// Register SkyX listeners
		mRoot->addFrameListener(mSkyX);
		// Since our two viewports are created through the mWindow render window, we've just add SkyX as a RenderTargetListener
		// and SkyX will automatically handle all the multi-camera stuff.
		// In very specific applications(like editors or when you're using a complex rendering pipeline), you'll need to manually
		// update the SkyX geometry instead of handle it by using listeners. In these situations just invoke SkyX::notifyCameraRender(...)
		// before rendering the camera frame.
		mWindow->addListener(mSkyX);

		setPreset(mPresets[mCurrentPreset]);
    }

	void chooseSceneManager()
    {
        // Create the SceneManager, in this case a generic one
		mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
        mSceneMgr->addRenderQueueListener(getOverlaySystem());
    }

	// Create text area for SkyX parameters
	void createTextArea()
	{
		// Create a panel
		Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(
			OverlayManager::getSingleton().createOverlayElement("Panel", "SkyXParametersPanel"));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setPosition(10, 10);
		panel->setDimensions(400, 400);

		// Create a text area
		mTextArea = static_cast<Ogre::TextAreaOverlayElement*>(
			OverlayManager::getSingleton().createOverlayElement("TextArea", "SkyXParametersTextArea"));
		mTextArea->setMetricsMode(Ogre::GMM_PIXELS);
		mTextArea->setPosition(0, 0);
		mTextArea->setDimensions(100, 100);
		mTextArea->setCaption("SkyX plugin demo");
		mTextArea->setCharHeight(16);
		mTextArea->setFontName("BlueHighway");
		mTextArea->setColourBottom(ColourValue(0.3, 0.5, 0.3));
		mTextArea->setColourTop(ColourValue(0.5, 0.7, 0.5));

		// Create an overlay, and add the panel
		Ogre::Overlay* overlay = OverlayManager::getSingleton().create("OverlayName");
		overlay->add2D(panel);

		// Add the text area to the panel
		panel->addChild(mTextArea);

		// Show the overlay
		overlay->show();
	}
};

// ----------------------------------------------------------------------------
// Main function, just boots the application object
// ----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	try
	{
		SampleApp app;
		app.go();
	}
	catch (Ogre::Exception &e)
	{
		std::cerr << "An exception has occured: " << e.getFullDescription();

		return -1;
	}

	return 0;
}
