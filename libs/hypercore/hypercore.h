#ifndef __HYPERCORE_H__
#define __HYPERCORE_H__

#include <4dm.h>

#include "ItemController.h"
#include "RecipeController.h"
#include "SoundController.h"
#include "EntityController.h"
#include "PlayerController.h"

namespace hypercore {
	class HyperCore {
	public:
		static bool startConsoleFlag;
		static void initMod();
	};
	
}

#endif //__HYPERCORE_H__