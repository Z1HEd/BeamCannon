#pragma once

#pragma once
#include "hypercore.h"

using namespace fdm;

namespace hypercore {
	class PlayerController {
	public:
		static std::unique_ptr<Item>& getMainHandItem(Player* player) {
			return *player->hotbar.getSlot(player->hotbar.selectedIndex);
		}

		static std::unique_ptr<Item>& getOffHandItem(Player* player) {
			return *player->equipment.getSlot(0);
		}
	};
}