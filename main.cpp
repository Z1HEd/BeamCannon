#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>
#include "4DKeyBinds.h"
#include "ItemBeamCannon.h"

initDLL

using namespace fdm;

std::vector<std::string> materials = {
	"Flawless Red Lens",
	"Flawless Green Lens",
	"Flawless Blue Lens",
	"Deadly Casing",
	"Iron Plate",
	"Solenoid Wire",
	"Beam Concentrator"
};

std::vector<std::string> upgrades = {
	"4D Optics Upgrade"
};

std::vector<nlohmann::json> recipes = {};

std::string chippingSoundFail = "assets/ChippingFail.ogg";
std::string chippingSoundSuccess= "assets/ChippingSuccess.ogg";

void spawnEntity(World* world, std::unique_ptr<Entity>& entity) {
	static glm::vec4 entityPos;
	static Chunk* chunk;
	entityPos = entity->getPos();
	chunk = world->getChunkFromCoords(entityPos.x, entityPos.z, entityPos.w);
	if (chunk) world->addEntityToChunk(entity, chunk);
}
void spawnEntityItem(World* world, const std::string& itemName, glm::vec4& position, glm::vec4& velocity) {
	std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
		Item::create(itemName, 1), position, velocity
	);
	spawnEntity(world, spawnedEntity);
}

// Lens Chipping mechanic
void chipLens(std::unique_ptr<Item>& lens, Player* player, World* world)
{
	lens->count--;
	if (lens->count == 0) lens.release();

	if (rand() % 3 != 0) {
		AudioManager::playSound4D(chippingSoundFail, "ambience", player->cameraPos, player->vel);
		return;
	}

	AudioManager::playSound4D(chippingSoundSuccess, "ambience", player->cameraPos, player->vel);

	spawnEntityItem(world, std::string("Flawless ") + lens->getName().c_str(), player->cameraPos, player->vel);
}
$hook(void, Player, mouseButtonInput, GLFWwindow* window, World* world, int button, int action, int mods) {

	if (button != GLFW_MOUSE_BUTTON_2 || action != GLFW_PRESS) return original(self, window, world, button, action, mods);

	std::unique_ptr<Item>& mainHandItem = *self->hotbar.getSlot(self->hotbar.selectedIndex);
	std::unique_ptr<Item>& offHandItem = *self->equipment.getSlot(0);

	if (mainHandItem == nullptr || offHandItem == nullptr) return original(self, window, world, button, action, mods);

	if (mainHandItem->getName() == "Rock" && (
		offHandItem->getName() == "Red Lens" ||
		offHandItem->getName() == "Green Lens" ||
		offHandItem->getName() == "Blue Lens")
		) chipLens(offHandItem,self,world);

	if (offHandItem->getName() == "Rock" && (
		mainHandItem->getName() == "Red Lens" ||
		mainHandItem->getName() == "Green Lens" ||
		mainHandItem->getName() == "Blue Lens")
		) chipLens(mainHandItem, self, world);
}


//Deadly text effect for materials
$hook(bool, ItemMaterial, isDeadly)
{
	if (self->getName() == "Deadly Casing") return true;

	return original(self);
}

// Render materials icons
$hook(void, ItemMaterial, render, const glm::ivec2& pos)
{
	auto index = std::find(materials.begin(), materials.end(),self->getName()) - materials.begin();

	if (index == materials.size())
		return original(self,pos);

	TexRenderer& tr = *ItemTool::tr; // or TexRenderer& tr = ItemTool::tr; after 4dmodding 2.2
	FontRenderer& fr = *ItemMaterial::fr;

	const Tex2D* ogTex = tr.texture; // remember the original texture

	static std::string iconPath = "";
	iconPath = std::format("{}{}.png", "assets/", self->getName().c_str());
	iconPath.erase(remove(iconPath.begin(), iconPath.end(), ' '), iconPath.end());

	tr.texture = ResourceManager::get(iconPath, true); // set to custom texture
	tr.setClip(0, 0, 36, 36);
	tr.setPos(pos.x, pos.y, 70, 72);
	tr.render();
	tr.texture = ogTex; // return to the original texture
}

// Handle Beam Cannon Upgrades 
$hook(bool, InventoryManager, applyTransfer, InventoryManager::TransferAction action, std::unique_ptr<Item>& selectedSlot, std::unique_ptr<Item>& cursorSlot, Inventory* other) {
	// How the fuck does this work
	
	InventoryManager& actualInventoryManager = StateGame::instanceObj->player.inventoryManager; // self is bullshit, when taking stuff its nullptr lol


	ItemBeamCannon* beamCannon = dynamic_cast<ItemBeamCannon*>(StateGame::instanceObj->player.hotbar.getSlot(StateGame::instanceObj->player.hotbar.selectedIndex)->get());
	if (beamCannon==nullptr) 
		beamCannon = dynamic_cast<ItemBeamCannon*>(StateGame::instanceObj->player.equipment.getSlot(0)->get());
	if (beamCannon == nullptr) return original(self, action, selectedSlot, cursorSlot, other);
	

	if (cursorSlot &&
		actualInventoryManager.secondary != nullptr &&
		actualInventoryManager.secondary->name == "beamCannonInventory" &&
		actualInventoryManager.secondary != other &&
		dynamic_cast<BeamCannonUpgrade*>(cursorSlot.get()) != nullptr &&
		!dynamic_cast<BeamCannonUpgrade*>(cursorSlot.get())->canBePutInto(dynamic_cast<InventoryGrid*>(actualInventoryManager.secondary)))
			return true; // Dont put incompatible upgrades

	auto result = original(self, action, selectedSlot, cursorSlot, other);
	beamCannon->reloadUpgrades();
	return result;
}


//Init stuff
$hookStatic(void, CraftingMenu, loadRecipes)
{
	static bool recipesLoaded = false;

	if (recipesLoaded) return;

	recipesLoaded = true;

	original();

	if (recipes.empty()) return;

	for (const auto& recipe : recipes) {
		if (std::any_of(CraftingMenu::recipes->begin(),
			CraftingMenu::recipes->end(),
			[&recipe](const nlohmann::json& globalRecipe) {
				return globalRecipe == recipe;
			})) continue;
		CraftingMenu::recipes->push_back(recipe);
	}
}
void addRecipe(const std::string& resultName, int resultCount,
	const std::vector<std::pair<std::string, int>>& components) {

	nlohmann::json recipeJson;
	recipeJson["result"] = { {"name", resultName}, {"count", resultCount} };

	nlohmann::json recipeComponents = nlohmann::json::array();
	for (const auto& [name, count] : components) {
		recipeComponents.push_back({ {"name", name}, {"count", count} });
	}

	recipeJson["recipe"] = recipeComponents;
	recipes.push_back(recipeJson);
}
void InitRecipes() {

	addRecipe("Deadly Casing", 1, { {"Deadly Bars",4} });
	addRecipe("Iron Plate", 1, { {"Iron Bars",3} });
	addRecipe("Solenoid Wire", 1, { {"Solenoid Bars",2} });

	addRecipe("Beam Concentrator", 1, { {"Flawless Blue Lens",1},{"Flawless Red Lens",1},{"Flawless Green Lens",1},{"Iron Plate",1} });
	addRecipe("Beam Cannon", 1, { {"Beam Concentrator",1},{"Deadly Casing",2},{"Iron Plate",2},{"Solenoid Wire",3} });

	addRecipe("4D Optics Upgrade", 1, { {"Iron Plate",2},{"4D Glasses",1} });
}
void InitBlueprints() {
	// Materials
	for (int i = 0;i < materials.size(); i++)
		(*Item::blueprints)[materials[i]] =
	{
		{ "type", "material"},
		{ "baseAttributes", nlohmann::json::object()}
	};

	// Beam Cannon
	(*Item::blueprints)["Beam Cannon"] =
	{
		{ "type", "beamCannon"},
		{ "baseAttributes", { { "inventory", nlohmann::json::array()}}}
	};

	//Beam Cannon Upgrades
	for (int i = 0;i < upgrades.size(); i++)
		(*Item::blueprints)[upgrades[i]] =
	{
		{ "type", "beamCannonUpgrade"},
		{ "baseAttributes", nlohmann::json::object()}
	};
}
void InitSounds() {
	chippingSoundFail = std::format("../../{}/{}", fdm::getModPath(fdm::modID), chippingSoundFail);
	chippingSoundSuccess = std::format("../../{}/{}", fdm::getModPath(fdm::modID), chippingSoundSuccess);
}

$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	//Initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();


	InitBlueprints();

	InitRecipes();

	InitSounds();
}
//Keybinds
void openBeamCannonInventory(GLFWwindow* window, int action, int mods)
{
	Player* player = &StateGame::instanceObj->player;
	if (player == nullptr || player->inventoryManager.isOpen()) return;

	ItemBeamCannon* beamCannon;
	beamCannon = dynamic_cast<ItemBeamCannon*>(player->hotbar.getSlot(player->hotbar.selectedIndex)->get());
	if (!beamCannon) beamCannon = dynamic_cast<ItemBeamCannon*>(player->equipment.getSlot(0)->get());
	if (!beamCannon) return;
	beamCannon->openInventory(player);
}
$exec
{
	KeyBinds::addBind("Beam Cannon", "Open Beam Cannon Upgrades", glfw::Keys::R, KeyBindsScope::PLAYER, openBeamCannonInventory);
}