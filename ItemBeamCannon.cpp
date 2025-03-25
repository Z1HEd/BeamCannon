#include "ItemBeamCannon.h"
#include "Upgrades/BeamCannonUpgrade.h"

using namespace fdm;

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

bool ItemBeamCannon::handleInputs(Player* player) {
	return false;
}

void ItemBeamCannon::openInventory(Player* player) {
	if (player->inventoryManager.isOpen()) return;
	player->inventoryManager.primary = &player->playerInventory;
	player->shouldResetMouse = true;
	player->inventoryManager.secondary = &inventory;


	player->inventoryManager.craftingMenu.updateAvailableRecipes();
	player->inventoryManager.updateCraftingMenuBox();

	inventorySession.inventory = &inventory;
	inventorySession.manager = &player->inventoryManager;
}

bool ItemBeamCannon::action(World*world, Player* player, int action) {
	return handleInputs(&StateGame::instanceObj->player);
}

void ItemBeamCannon::render(const glm::ivec2& pos) {
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



void ItemBeamCannon::renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) {
	if (hasCompassEffect)
		CompassRenderer::renderHand(glm::mat4x4{ {0,0,0,1},{0,0,0,0},{0,0,1,0},{1,0,0,0} });
	
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
	result->inventory.name = "beamCannonInventory";
	result->inventory.label = "Beam Cannon Upgrades:";
	result->inventory.renderPos= glm::ivec2{ 397,50 };
	result->count = 1;

	result->applyUpgrades();

	return result;
}

// Glasses effect
$hook(bool, Player, isHoldingGlasses) {
	ItemBeamCannon* beamCannon;
	beamCannon = dynamic_cast<ItemBeamCannon*>(self->hotbar.getSlot(self->hotbar.selectedIndex)->get());
	if (!beamCannon) beamCannon = dynamic_cast<ItemBeamCannon*>(self->equipment.getSlot(0)->get());
	if (!beamCannon) return original(self);
	return original(self) || beamCannon->hasGlassesEffect;
}