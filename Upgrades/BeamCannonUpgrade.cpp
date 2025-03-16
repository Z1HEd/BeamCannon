#include "BeamCannonUpgrade.h"
#include "OpticsUpgrade.h"

bool BeamCannonUpgrade::isDeadly() {
	return false;
}
uint32_t BeamCannonUpgrade::getStackLimit() {
	return 1;
}
bool BeamCannonUpgrade::action(World* world, Player* player, int action) {

	return true;
}
void BeamCannonUpgrade::renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) {
}

nlohmann::json BeamCannonUpgrade::saveAttributes() {
	return nlohmann::json::object(); // Save attributes you need.
}

// Instantiating item
$hookStatic(std::unique_ptr<Item>, Item, instantiateItem, const stl::string& itemName, uint32_t count, const stl::string& type, const nlohmann::json& attributes) {

	// Dont forget to change "type" to type of your item (the same as in ItemManager::addItem(name,TYPE))
	if (type != "beamCannonUpgrade") return original(itemName, count, type, attributes);

	
	std::unique_ptr<Item> result= nullptr;

	if (itemName == "4D Optics Upgrade")
		result = std::make_unique<OpticsUpgrade>();


	return result;
}