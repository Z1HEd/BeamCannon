#ifndef HYPERCORE_H
#define HYPERCORE_H


#include <4dm.h>

#include "ItemManager.h"
#include "RecipeManager.h"
#include "SoundLoader.h"

using namespace fdm;

initDLL

void gameInit();

namespace hypercore {
	$hook(void, StateIntro, init, StateManager& s)
	{
		original(self, s);

		//Initialize opengl stuff
		glewExperimental = true;
		glewInit();
		glfwInit();

		gameInit();


		ItemManager::initBlueprints();

		SoundLoader::loadSounds();
	}
}



#endif //HYPERCORE_H