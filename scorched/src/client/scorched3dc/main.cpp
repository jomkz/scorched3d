#include "Scorched3DC.h"
#include <stdlib.h>
#include <common/main.h>
#include <client/ClientParams.h>

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
	{
		run_main(__argc, __argv, *ClientParams::instance());
#else
	int main(int argc, char *argv[])
	{
		run_main(argc, argv, *ClientParams::instance());
#endif

		Scorched3DC app;
		try {
			app.go();
		} catch( Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
			return 1;
		}  catch (CEGUI::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, e.getMessage().c_str(), "A CEGUI exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "A CEGUI exception has occured: " <<
				e.getMessage().c_str() << std::endl;
#endif
			return 1;
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif
