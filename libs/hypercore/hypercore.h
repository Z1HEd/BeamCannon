#ifndef HYPERCORE_H
#define HYPERCORE_H


#include <4dm.h>

#include "ItemController.h"
#include "RecipeController.h"
#include "SoundController.h"
#include "EntityController.h"
#include "PlayerController.h"

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


		ItemController::initBlueprints();

		SoundController::loadSounds();
	}
}



#endif //HYPERCORE_H