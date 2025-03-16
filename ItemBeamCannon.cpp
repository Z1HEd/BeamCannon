#include "ItemBeamCannon.h"

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

bool ItemBeamCannon::action(World* world, Player* player, int action) {
	
	// actions are strange, better use player->keys
	
	return true; // return true if your code did something
}

void ItemBeamCannon::renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) {
	// Render item model
}

nlohmann::json ItemBeamCannon::saveAttributes() {
	return nlohmann::json::object(); // Save attributes you need.
}

// Cloning item
std::unique_ptr<Item> ItemBeamCannon::clone() {
	auto result = std::make_unique<ItemBeamCannon>();

	// Assign all the needed variables here

	return result;
}

// Instantiating item
$hookStatic(std::unique_ptr<Item>, Item, instantiateItem, const stl::string& itemName, uint32_t count, const stl::string& type, const nlohmann::json& attributes) {
	
	// Dont forget to change "type" to type of your item (the same as in ItemController::addItem(name,TYPE))
	if (type != "beamCannon") return original(itemName, count, type, attributes);

	auto result = std::make_unique<ItemBeamCannon>();
	
	// Assign all the needed variables here

	return result;
}