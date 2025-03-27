#pragma once

#include <4dm.h>
#include "Upgrades/BeamCannonUpgrade.h"

using namespace fdm;

class ItemBeamCannon : public Item {
public:

	static std::string fuelFlushSound = "assets/FuelFlush.ogg";

	bool hasGlassesEffect;
	bool hasCompassEffect;
	float effectiveDistance;
	float entityDamage;
	float fuelUsage;

	float fuelLevel = 0;
	bool isFuelDeadly = false;
	bool isSelectedFuelDeadly = false;
	bool isFlushing = false;

	InventoryGrid inventory;
	InventorySession inventorySession{};
	static MeshRenderer renderer;

	void resetUpgrades();
	void applyUpgrades();
	void reloadUpgrades();

	int getSelectedFuelCount(InventoryPlayer& inventory);
	void consumeSelectedFuel(InventoryPlayer& inventory);
	static void rendererInit();

	void openInventory(Player*player);

	void update(Player*,double);
	void tryShooting(Player*, double);

	// Virtual functions overrides
	bool isCompatible(const std::unique_ptr<Item>& other) override;
	stl::string getName() override;
	bool isDeadly() override;
	uint32_t getStackLimit() override;
	void render(const glm::ivec2& pos) override;
	void renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) override;
	std::unique_ptr<Item> clone() override;
	nlohmann::json saveAttributes() override;
	
};