#pragma once

#pragma once
#include "hypercore.h"

using namespace fdm;

namespace hypercore {
	class PlayerController {
	public:
		inline static InventorySession inventorySession{};

		inline static std::unique_ptr<Item>& getMainHandItem(Player* player) {
			return *player->hotbar.getSlot(player->hotbar.selectedIndex);
		}

		inline static std::unique_ptr<Item>& getOffHandItem(Player* player) {
			return *player->equipment.getSlot(0);
		}

		inline static bool isInventoryOpen(Player* player) {
			return player->inventoryManager.isOpen();
		}

		inline static bool openInventory(Player* player,InventoryGrid& inventory) {
			if (player->inventoryManager.isOpen()) return false;

			player->inventoryManager.primary = &player->playerInventory;
			player->shouldResetMouse = true;
			player->inventoryManager.secondary = &inventory;


			player->inventoryManager.craftingMenu.updateAvailableRecipes();
			player->inventoryManager.updateCraftingMenuBox();

			inventorySession.inventory = &inventory;
			inventorySession.manager = &player->inventoryManager;
			return true;
		}
	};
}