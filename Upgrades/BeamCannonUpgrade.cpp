#include "BeamCannonUpgrade.h"
#include "OpticsUpgrade.h"
#include "GyroscopeUpgrade.h"

bool BeamCannonUpgrade::isDeadly() {
	return false;
}
uint32_t BeamCannonUpgrade::getStackLimit() {
	return 1;
}
void BeamCannonUpgrade::renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) {
}

nlohmann::json BeamCannonUpgrade::saveAttributes() {
	return nlohmann::json::object(); // Save attributes you need.
}

bool BeamCannonUpgrade::canBePutInto(InventoryGrid* inventory) {
	for (auto& slot : inventory->slots) {
		BeamCannonUpgrade* upgradeInSlot = dynamic_cast<BeamCannonUpgrade*>(slot.get());
		if (upgradeInSlot == nullptr) continue;
		if (!isCompatibleUpgrade(upgradeInSlot) || !upgradeInSlot->isCompatibleUpgrade(this)) return false;
	}
	return true;
}

void BeamCannonUpgrade::render(const glm::ivec2& pos) {
	TexRenderer& tr = *ItemTool::tr; // or TexRenderer& tr = ItemTool::tr; after 4dmodding 2.2
	FontRenderer& fr = *ItemMaterial::fr;

	const Tex2D* ogTex = tr.texture; // remember the original texture

	static std::string iconPath = "";
	iconPath = std::format("{}{}.png", "assets/", getName().c_str());
	iconPath.erase(remove(iconPath.begin(), iconPath.end(), ' '), iconPath.end());

	tr.texture = ResourceManager::get(iconPath, true); // set to custom texture
	tr.setClip(0, 0, 36, 36);
	tr.setPos(pos.x, pos.y, 70, 72);
	tr.render();
	tr.texture = ogTex; // return to the original texture
}

// Instantiating item
$hookStatic(std::unique_ptr<Item>, Item, instantiateItem, const stl::string& itemName, uint32_t count, const stl::string& type, const nlohmann::json& attributes) {

	if (type != "beamCannonUpgrade") return original(itemName, count, type, attributes);

	
	std::unique_ptr<Item> result= nullptr;

	if (itemName == "4D Optics Upgrade")
		result = std::make_unique<OpticsUpgrade>();
	else if (itemName == "Gyroscope Upgrade")
		result = std::make_unique<GyroscopeUpgrade>();

	return result;
}
