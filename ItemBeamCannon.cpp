#include "libs/hypercore/hypercore.h"
#include "ItemBeamCannon.h"
#include "Upgrades/BeamCannonUpgrade.h"

using namespace hypercore;

bool ItemBeamCannon::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<ItemBeamCannon*>(other.get());
}

stl::string ItemBeamCannon::getName() {
	return "Beam Cannon";
}

bool ItemBeamCannon::isDeadly() {
	return true; // Used for fancy text effect 
}

uint32_t ItemBeamCannon::getStackLimit() {
	return 1; 
}

void ItemBeamCannon::resetUpgrades() {
	hasGlassesEffect = false;
	hasCompassEffect = false;
}
void ItemBeamCannon::applyUpgrades() {
	for (int i = 0;i < inventory.getSlotCount(); i++) {
		if (auto* upgrade = dynamic_cast<BeamCannonUpgrade*>(inventory.getSlot(i)->get()))
			upgrade->applyUpgrade(this);
	}
}
void ItemBeamCannon::reloadUpgrades() {
	resetUpgrades();
	applyUpgrades();
}

bool ItemBeamCannon::action(World* world, Player* player, int action) {
	Console::printLine(action);
	if (player->keys.rightMouseDown) {
		PlayerController::openInventory(player, inventory);
		return true;
	}

	return true;
}

void ItemBeamCannon::renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) {
	// Render item model
}

nlohmann::json ItemBeamCannon::saveAttributes() {
	return {{ "inventory", inventory.save()}}; 
}

// Cloning item
std::unique_ptr<Item> ItemBeamCannon::clone() {
	auto result = std::make_unique<ItemBeamCannon>();

	result->inventory = inventory;

	return result;
}

// Instantiating item
$hookStatic(std::unique_ptr<Item>, Item, instantiateItem, const stl::string& itemName, uint32_t count, const stl::string& type, const nlohmann::json& attributes) {
	if (type != "beamCannon") return original(itemName, count, type, attributes);

	auto result = std::make_unique<ItemBeamCannon>();
	
	result->inventory = InventoryGrid({ 2,3 });
	result->inventory.load(attributes["inventory"]);
	result->inventory.label = "Beam Cannon Upgrades:";
	result->inventory.renderPos= glm::ivec2{ 397,50 };

	return result;
}