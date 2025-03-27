#include "ItemBeamCannon.h"
#include "Upgrades/BeamCannonUpgrade.h"
#include "EntityController.h"

using namespace fdm;
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
	effectiveDistance = 10;
	entityDamage = 5;
	fuelUsage = 0.01;
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

int ItemBeamCannon::getSelectedFuelCount(InventoryPlayer& inventory) {
	int count = 0;
	for (int slot = 0;slot < inventory.getSlotCount(); slot++) {
		Item* i = inventory.getSlot(slot)->get();
		if (i != nullptr && i->getName() == (isSelectedFuelDeadly ? "Deadly Fuel" : "Biofuel"))
			count += i->count;

	}
	return count;
}
void ItemBeamCannon::consumeSelectedFuel(InventoryPlayer& inventory) {
	for (int slot = 0;slot < inventory.getSlotCount(); slot++) {
		Item* i = inventory.getSlot(slot)->get();
		if (i != nullptr && i->getName() == (isSelectedFuelDeadly ? "Deadly Fuel" : "Biofuel")) {
			i->count--;
			if (i->count < 1) inventory.getSlot(slot)->reset();
			fuelLevel = 1.0f;
			isFuelDeadly = isSelectedFuelDeadly;
			return;
		}
	}
}
void ItemBeamCannon::rendererInit() {
	MeshBuilder mesh{ BlockInfo::HYPERCUBE_FULL_INDEX_COUNT };
	// vertex position attribute
	mesh.addBuff(BlockInfo::hypercube_full_verts, sizeof(BlockInfo::hypercube_full_verts));
	mesh.addAttr(GL_UNSIGNED_BYTE, 4, sizeof(glm::u8vec4));
	// per-cell normal attribute
	mesh.addBuff(BlockInfo::hypercube_full_normals, sizeof(BlockInfo::hypercube_full_normals));
	mesh.addAttr(GL_FLOAT, 1, sizeof(GLfloat));

	mesh.setIndexBuff(BlockInfo::hypercube_full_indices, sizeof(BlockInfo::hypercube_full_indices));

	renderer.setMesh(&mesh);
}

static void destroyBlock(World* world,const glm::ivec4& blockPos) {
	EntityController::spawnEntityItem(world,BlockInfo::blockNames->find(world->getBlock(blockPos))->second, glm::vec4 { 0.5 }+ glm::vec4(blockPos));
	world->setBlockUpdate(blockPos, BlockInfo::TYPE::AIR);
}

void ItemBeamCannon::tryShooting(Player* player,double dt) {
	static double timeSinceLastTick = 0;
	constexpr double timeBetweenTicks = 0.35; // ~3 ticks per second
	timeSinceLastTick += dt;

	if (timeSinceLastTick < timeBetweenTicks) return;

	if (isFlushing && fuelLevel > 0.0f) {
		AudioManager::playSound4D(flushSound, "ambience", player->cameraPos, { 0,0,0,0 });
		fuelLevel = 0.0f;
	}

	if (fuelLevel <= 0.0f && player->touchingGround && !isFlushing) consumeSelectedFuel(player->inventoryAndEquipment);

	fuelLevel -= fuelUsage;

	timeSinceLastTick = 0;

	glm::vec4 reachStartpoint = player->cameraPos;
	glm::vec4 reachEndpoint = player->cameraPos + player->forward * effectiveDistance;

	Entity* intersect = StateGame::instanceObj->world->getEntityIntersection(reachStartpoint, reachEndpoint, player->EntityPlayerID);

	glm::ivec4 blockPos = reachStartpoint;
	glm::ivec4 endPos;

	if (intersect != nullptr) {
		intersect->takeDamage(entityDamage, StateGame::instanceObj->world.get());
		return;
	}
	else if (StateGame::instanceObj->world->castRay(reachStartpoint, blockPos, endPos, reachEndpoint)) {
		destroyBlock(StateGame::instanceObj->world.get(), endPos);
	}

}

void ItemBeamCannon::update(Player* player,double dt) {
	if (player->keys.rightMouseDown)
		tryShooting(player, dt);
}

$hook(void, Player, update, World* world, double _, EntityPlayer* entityPlayer) { // dt is useless bcs its hardcoded to 0.01
	original(self, world, _, entityPlayer);

	ItemBeamCannon* cannon;
	cannon = dynamic_cast<ItemBeamCannon*>(self->hotbar.getSlot(self->hotbar.selectedIndex)->get());
	if (!cannon) cannon = dynamic_cast<ItemBeamCannon*>(self->equipment.getSlot(0)->get());
	if (!cannon) return;

	static double lastTime = glfwGetTime() - 0.01;
	double curTime = glfwGetTime();
	double dt = curTime - lastTime;
	lastTime = curTime;

	cannon->update(self, dt);
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
	return {{ "inventory", inventory.save()},{ "fuelLevel", fuelLevel }, { "isFuelDeadly", isFuelDeadly}, { "isSelectedFuelDeadly", isSelectedFuelDeadly} };
}

// Cloning item
std::unique_ptr<Item> ItemBeamCannon::clone() {
	auto result = std::make_unique<ItemBeamCannon>();

	result->inventory = inventory;
	result->fuelLevel = fuelLevel;
	result->isFuelDeadly = isFuelDeadly;
	result->isSelectedFuelDeadly = isSelectedFuelDeadly;

	result->reloadUpgrades();

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

	result->fuelLevel = (float)attributes["fuelLevel"];
	result->isFuelDeadly = (bool)attributes["isFuelDeadly"];
	result->isSelectedFuelDeadly = (bool)attributes["isSelectedFuelDeadly"];

	result->inventory.renderPos= glm::ivec2{ 397,50 };
	result->count = 1;

	result->reloadUpgrades();

	return result;
}

// Glasses/Compass effect
$hook(bool, Player, isHoldingGlasses) {
	ItemBeamCannon* beamCannon;
	beamCannon = dynamic_cast<ItemBeamCannon*>(self->hotbar.getSlot(self->hotbar.selectedIndex)->get());
	if (!beamCannon) beamCannon = dynamic_cast<ItemBeamCannon*>(self->equipment.getSlot(0)->get());
	if (!beamCannon) return original(self);
	return original(self) || beamCannon->hasGlassesEffect;
}