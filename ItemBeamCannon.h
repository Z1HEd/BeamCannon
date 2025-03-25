#pragma once

#include <4dm.h>
#include "Upgrades/BeamCannonUpgrade.h"

using namespace fdm;

class ItemBeamCannon : public Item {
public:
	bool hasGlassesEffect;
	bool hasCompassEffect;

	InventoryGrid inventory;
	InventorySession inventorySession{};

	void resetUpgrades();
	void applyUpgrades();
	void reloadUpgrades();

	void openInventory(Player*player);

	bool handleInputs(Player*);

	// Virtual functions overrides
	bool isCompatible(const std::unique_ptr<Item>& other) override;
	stl::string getName() override;
	bool isDeadly() override;
	uint32_t getStackLimit() override;
	bool action(World* world, Player* player, int action) override;
	void render(const glm::ivec2& pos) override;
	void renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) override;
	std::unique_ptr<Item> clone() override;
	nlohmann::json saveAttributes() override;
	
};