#pragma once

#include <4dm.h>
#include "Upgrades/BeamCannonUpgrade.h"

using namespace fdm;

class ItemBeamCannon : public Item {
public:

	static std::string laserSound;

	bool hasGlassesEffect;
	bool hasCompassEffect;
	bool hasMagnetEffect;
	float effectiveDistance;
	float entityDamage;
	float diggingPower;
	float fuelUsage;
	int diggingArea;

	double timeSinceLastTick = 0;
	float fuelLevel = 0;
	bool isFuelDeadly = false;
	bool isSelectedFuelDeadly = false;

	InventoryGrid inventory;
	InventorySession inventorySession{};
	static MeshRenderer renderer;
	

	void resetUpgrades();
	void applyUpgrades();
	void reloadUpgrades();

	int getSelectedFuelCount(InventoryPlayer& inventory);
	void consumeSelectedFuel(InventoryPlayer& inventory);
	float getLaserLength();

	static void rendererInit();

	void openInventory(Player*player);

	void destroyBlock(World* world, const glm::ivec4& blockPos, const glm::vec4& dropPos);
	void destroyArea(World* world, const glm::ivec4& centerPos,const int& size);

	void update(Player*,double);
	bool tryShooting(Player*, double);

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