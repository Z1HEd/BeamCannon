#include "ItemTemplate.h"

bool ItemTemplate::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<ItemTemplate*>(other.get());
}

stl::string ItemTemplate::getName() {
	return "ItemName";
}

bool ItemTemplate::isDeadly() { 
	return true; // Used for fancy text effect 
}

uint32_t ItemTemplate::getStackLimit() {
	return 1; 
}

bool ItemTemplate::action(World* world, Player* player, int action) {
	
	// actions are strange, better use player->keys
	
	return true; // return true if your code did something
}

void ItemTemplate::renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) {
	// Render item model
}

nlohmann::json ItemTemplate::saveAttributes() {
	return nlohmann::json::object(); // Save attributes you need.
}

// Cloning item
std::unique_ptr<Item> ItemTemplate::clone() {
	auto result = std::make_unique<ItemTemplate>();

	// Assign all the needed variables here

	return result;
}

// Instantiating item
$hookStatic(std::unique_ptr<Item>, Item, instantiateItem, const stl::string& itemName, uint32_t count, const stl::string& type, const nlohmann::json& attributes) {
	
	// Dont forget to change "type" to type of your item (the same as in ItemManager::addItem(name,TYPE))
	if (type != "type") return original(itemName, count, type, attributes);

	auto result = std::make_unique<ItemTemplate>();
	
	// Assign all the needed variables here

	return result;
}