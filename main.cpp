#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>
#include "4DKeyBinds.h"
#include "EntityController.h"
#include "ItemBeamCannon.h"

initDLL

using namespace fdm;
using namespace hypercore;

std::vector<std::string> materials = {
	"Flawless Red Lens",
	"Flawless Green Lens",
	"Flawless Blue Lens",
	"Biofuel",
	"Deadly Fuel",
	"Deadly Casing",
	"Iron Plate",
	"Solenoid Wire",
	"Gyroscope",
	"Beam Concentrator"
};

std::vector<std::string> upgrades = {
	"4D Optics Upgrade",
	"Gyroscope Upgrade",
	"Area Upgrade",
	"Concentrator Upgrade",
	"Coils Upgrade",
	"Filtration Upgrade",
	"Magnet Upgrade",
	"Coils Upgrade",
	"Matter Disassembler Upgrade"
};

std::vector<nlohmann::json> recipes = {};

//Sounds
std::string chippingSoundFail = "assets/ChippingFail.ogg";
std::string chippingSoundSuccess= "assets/ChippingSuccess.ogg";
std::string fuelSwitchSound = "assets/FuelSwitch.ogg";

// UI
QuadRenderer qr{};
TexRenderer fuelBackgroundRenderer;
TexRenderer biofuelRenderer;
TexRenderer deadlyFuelRenderer;
gui::Text fuelCountText;
gui::Interface ui;
FontRenderer font{};

// Dont deselect when open
$hook(void, WorldSingleplayer, localPlayerEvent, Player* player, Packet::ClientPacket eventType, int64_t eventValue, void* data) {
	if (eventType != Packet::C_HOTBAR_SLOT_SELECT) return original(self, player, eventType, eventValue, data);
	if (!player->inventoryManager.isOpen()) return original(self, player, eventType, eventValue, data);
	if (player->hotbar.getSlot(player->hotbar.selectedIndex)->getName()!="Beam Cannon") return original(self, player, eventType, eventValue, data);
	if (player->inventoryManager.secondary!=&((ItemBeamCannon*)player->hotbar.getSlot(player->hotbar.selectedIndex).get())->inventory) return original(self, player, eventType, eventValue, data);

}

// Only craft upgrades when beamcannon inventory is open
$hook(void, CraftingMenu, updateAvailableRecipes)
{
	original(self);

	bool isBeamCannonOpen = false;

	if (StateGame::instanceObj.player.hotbar.getSlot(
		StateGame::instanceObj.player.hotbar.selectedIndex)&&
		StateGame::instanceObj.player.hotbar.getSlot(
		StateGame::instanceObj.player.hotbar.selectedIndex)->getName() == "Beam Cannon"
		&& StateGame::instanceObj.player.inventoryManager.isOpen()
		&& StateGame::instanceObj.player.inventoryManager.secondary ==
		&((ItemBeamCannon*)StateGame::instanceObj.player.hotbar.getSlot(
			StateGame::instanceObj.player.hotbar.selectedIndex).get())->inventory
		) isBeamCannonOpen = true;


	for (auto it = self->craftableRecipes.begin(); it < self->craftableRecipes.end(); )
	{

		if (!isBeamCannonOpen && dynamic_cast<BeamCannonUpgrade*>(it->result.get())) {
			it = self->craftableRecipes.erase(it);
			continue;
		}
		if (isBeamCannonOpen && !dynamic_cast<BeamCannonUpgrade*>(it->result.get())) {
			it = self->craftableRecipes.erase(it);
			continue;
		}
		it++;
	}
	self->Interface->updateCraftingMenuBox();
}


// Init UI
void viewportCallback(void* user, const glm::ivec4& pos, const glm::ivec2& scroll)
{
	GLFWwindow* window = (GLFWwindow*)user;

	// update the render viewport
	int wWidth, wHeight;
	glfwGetWindowSize(window, &wWidth, &wHeight);
	glViewport(pos.x, wHeight - pos.y - pos.w, pos.z, pos.w);

	// create a 2D projection matrix from the specified dimensions and scroll position
	glm::mat4 projection2D = glm::ortho(0.0f, (float)pos.z, (float)pos.w, 0.0f, -1.0f, 1.0f);
	projection2D = glm::translate(projection2D, { scroll.x, scroll.y, 0 });

	// update all 2D shaders
	const Shader* textShader = ShaderManager::get("textShader");
	textShader->use();
	glUniformMatrix4fv(glGetUniformLocation(textShader->id(), "P"), 1, GL_FALSE, &projection2D[0][0]);

	const Shader* tex2DShader = ShaderManager::get("tex2DShader");
	tex2DShader->use();
	glUniformMatrix4fv(glGetUniformLocation(tex2DShader->id(), "P"), 1, GL_FALSE, &projection2D[0][0]);

	const Shader* quadShader = ShaderManager::get("quadShader");
	quadShader->use();
	glUniformMatrix4fv(glGetUniformLocation(quadShader->id(), "P"), 1, GL_FALSE, &projection2D[0][0]);
}

$hook(void, StateGame, init, StateManager& s)
{
	original(self, s);

	font = { ResourceManager::get("pixelFont.png"), ShaderManager::get("textShader") };

	qr.shader = ShaderManager::get("quadShader");
	qr.init();

	fuelBackgroundRenderer.texture = ItemBlock::tr.texture;
	fuelBackgroundRenderer.shader = ShaderManager::get("tex2DShader");
	fuelBackgroundRenderer.init();

	biofuelRenderer.texture = ResourceManager::get("assets/Biofuel.png", true);
	biofuelRenderer.shader = ShaderManager::get("tex2DShader");
	biofuelRenderer.init();

	deadlyFuelRenderer.texture = ResourceManager::get("assets/DeadlyFuel.png", true);
	deadlyFuelRenderer.shader = ShaderManager::get("tex2DShader");
	deadlyFuelRenderer.init();

	fuelCountText.size = 2;
	fuelCountText.text = "0";
	fuelCountText.shadow = true;

	// initialize the Interface
	ui = gui::Interface{ s.window };
	ui.viewportCallback = viewportCallback;
	ui.viewportUser = s.window;
	ui.font = &font;
	ui.qr = &qr;

	ui.addElement(&fuelCountText);
}

// Render UI
$hook(void, Player, renderHud, GLFWwindow* window) {
	original(self, window);

	static bool isBeamCannonOffhand;
	isBeamCannonOffhand = false;

	if (!&StateGame::instanceObj.player || StateGame::instanceObj.player.inventoryManager.isOpen()) return;
	ItemBeamCannon* beamCannon;
	beamCannon = dynamic_cast<ItemBeamCannon*>(StateGame::instanceObj.player.hotbar.getSlot(StateGame::instanceObj.player.hotbar.selectedIndex).get());
	if (!beamCannon) {
		beamCannon = dynamic_cast<ItemBeamCannon*>(StateGame::instanceObj.player.equipment.getSlot(0).get());
		isBeamCannonOffhand = true;
	}
	if (!beamCannon) return;

	glDisable(GL_DEPTH_TEST);

	int width, height;
	glfwGetWindowSize(window, &width, &height);


	int posX, posY;
	if (!isBeamCannonOffhand) {
		posX = self->hotbar.renderPos.x + 68 /*imperfect reality*/ + 34 * ((self->hotbar.selectedIndex + 1) % 2);
		posY = self->hotbar.renderPos.y + (self->hotbar.selectedIndex * 56);
	}
	else {
		posX = self->hotbar.renderPos.x + 78 + 34 * ((9) % 2);
		posY = self->hotbar.renderPos.y + (8 * 56) + 6;
	}

	fuelBackgroundRenderer.setPos(posX, posY, 72, 72);
	fuelBackgroundRenderer.setClip(0, 0, 36, 36);
	fuelBackgroundRenderer.setColor(1, 1, 1, 0.4f);
	fuelBackgroundRenderer.render();

	TexRenderer& selectedFuelRenderer = beamCannon->isSelectedFuelDeadly ? deadlyFuelRenderer : biofuelRenderer;
	selectedFuelRenderer.setPos(posX, posY, 72, 72);
	selectedFuelRenderer.setClip(0, 0, 36, 36);
	if (beamCannon->getSelectedFuelCount(self->inventoryAndEquipment) > 0)
		selectedFuelRenderer.setColor(1, 1, 1, 1);
	else
		selectedFuelRenderer.setColor(.2f, .2f, .2f, 1);
	selectedFuelRenderer.render();

	fuelCountText.text = std::to_string(beamCannon->getSelectedFuelCount(self->inventoryAndEquipment));
	fuelCountText.offsetX(posX + 45);
	fuelCountText.offsetY(posY + 45);


	ui.render();

	glEnable(GL_DEPTH_TEST);

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

	EntityController::spawnEntityItem(world, std::string("Flawless ") + lens->getName().c_str(), player->cameraPos, player->vel);
}
$hook(void, Player, mouseButtonInput, GLFWwindow* window, World* world, int button, int action, int mods) {

	if (button != GLFW_MOUSE_BUTTON_2 || action != GLFW_PRESS || self->inventoryManager.isOpen()) return original(self, window, world, button, action, mods);

	std::unique_ptr<Item>& mainHandItem = self->hotbar.getSlot(self->hotbar.selectedIndex);
	std::unique_ptr<Item>& offHandItem = self->equipment.getSlot(0);

	if (mainHandItem == nullptr || offHandItem == nullptr) return original(self, window, world, button, action, mods);

	if (mainHandItem->getName() == "Rock" && (
		offHandItem->getName() == "Red Lens" ||
		offHandItem->getName() == "Green Lens" ||
		offHandItem->getName() == "Blue Lens")
		) chipLens(offHandItem,self,world);

	else if (offHandItem->getName() == "Rock" && (
		mainHandItem->getName() == "Red Lens" ||
		mainHandItem->getName() == "Green Lens" ||
		mainHandItem->getName() == "Blue Lens")
		) chipLens(mainHandItem, self, world);

	else return original(self, window, world, button, action, mods);
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

	TexRenderer& tr = ItemTool::tr;
	FontRenderer& fr = ItemMaterial::fr;

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
	
	InventoryManager& actualInventoryManager = StateGame::instanceObj.player.inventoryManager; // self is bullshit, when taking stuff its nullptr lol


	ItemBeamCannon* beamCannon = dynamic_cast<ItemBeamCannon*>(StateGame::instanceObj.player.hotbar.getSlot(StateGame::instanceObj.player.hotbar.selectedIndex).get());
	if (beamCannon==nullptr) 
		beamCannon = dynamic_cast<ItemBeamCannon*>(StateGame::instanceObj.player.equipment.getSlot(0).get());
	if (beamCannon == nullptr) return original(self, action, selectedSlot, cursorSlot, other);
	

	if (cursorSlot &&
		actualInventoryManager.secondary != nullptr &&
		actualInventoryManager.secondary->name == "beamCannonInventory" &&
		actualInventoryManager.secondary != other &&
		dynamic_cast<BeamCannonUpgrade*>(cursorSlot.get()) == nullptr ||
		(dynamic_cast<BeamCannonUpgrade*>(cursorSlot.get()) != nullptr &&
		!dynamic_cast<BeamCannonUpgrade*>(cursorSlot.get())->canBePutInto(dynamic_cast<InventoryGrid*>(actualInventoryManager.secondary))))
			return true; // Dont put incompatible upgrades, or non- upgrade items

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
		if (std::any_of(CraftingMenu::recipes.begin(),
			CraftingMenu::recipes.end(),
			[&recipe](const nlohmann::json& globalRecipe) {
				return globalRecipe == recipe;
			})) continue;
		CraftingMenu::recipes.push_back(recipe);
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

	addRecipe("Klein Bottle", 1, { {"Glass",3} });
	addRecipe("Deadly Fuel", 1, { {"Klein Bottle",1},{"Deadly Ore",1}});

	addRecipe("Deadly Casing", 1, { {"Deadly Bars",4} });
	addRecipe("Iron Plate", 1, { {"Iron Bars",3} });
	addRecipe("Solenoid Wire", 1, { {"Solenoid Bars",2} });
	addRecipe("Gyroscope", 1, { {"Iron Bars",2},{"Compass",1} });

	addRecipe("Beam Concentrator", 1, { {"Flawless Blue Lens",1},{"Flawless Red Lens",1},{"Flawless Green Lens",1},{"Iron Plate",1} });
	addRecipe("Beam Cannon", 1, { {"Beam Concentrator",1},{"Deadly Casing",2},{"Iron Plate",2},{"Solenoid Wire",3} });

	addRecipe("4D Optics Upgrade", 1, { {"Iron Plate",2},{"4D Glasses",1} });
	addRecipe("Gyroscope Upgrade", 1, { {"Iron Plate",2},{"Gyroscope",1} });
	addRecipe("Area Upgrade", 1, { {"Iron Plate",2},{"Ultrahammer",1} });
	addRecipe("Concentrator Upgrade", 1, { {"Iron Plate",2},{"Beam Concentrator",1} });
	addRecipe("Coils Upgrade", 1, { {"Iron Plate",2},{"Solenoid Wire",2} });
	addRecipe("Filtration Upgrade", 1, { {"Iron Plate",2},{"Sand",20},{"Klein Bottle",5} });
	addRecipe("Magnet Upgrade", 1, { {"Iron Plate",2},{"Solenoid Collector",1} });
	addRecipe("Matter Disassembler Upgrade", 1, { {"Iron Plate",2},{"Deadly Pick",1} });
}
void InitBlueprints() {
	// Materials
	for (int i = 0;i < materials.size(); i++)
		(Item::blueprints)[materials[i]] =
	{
		{ "type", "material"},
		{ "baseAttributes", nlohmann::json::object()}
	};

	// Beam Cannon
	(Item::blueprints)["Beam Cannon"] =
	{
		{ "type", "beamCannon"},
		{ "baseAttributes", {{"fuelLevel",0.0f}, {"inventory", nlohmann::json::array()},{"isFuelDeadly",false},{"isSelectedFuelDeadly",false}}}
	};

	//Beam Cannon Upgrades
	for (int i = 0;i < upgrades.size(); i++)
		(Item::blueprints)[upgrades[i]] =
	{
		{ "type", "beamCannonUpgrade"},
		{ "baseAttributes", nlohmann::json::object()}
	};
}
void InitSounds() {
	chippingSoundFail = std::format("../../{}/{}", fdm::getModPath(fdm::modID), chippingSoundFail);
	chippingSoundSuccess = std::format("../../{}/{}", fdm::getModPath(fdm::modID), chippingSoundSuccess);
	fuelSwitchSound = std::format("../../{}/{}", fdm::getModPath(fdm::modID), fuelSwitchSound);
	ItemBeamCannon::laserSound = std::format("../../{}/{}", fdm::getModPath(fdm::modID), "assets/LaserSound.ogg");


	if (!AudioManager::loadSound(chippingSoundFail)) Console::printLine("Cannot load sound: ", chippingSoundFail);
	if (!AudioManager::loadSound(chippingSoundSuccess)) Console::printLine("Cannot load sound: ", chippingSoundSuccess);
	if (!AudioManager::loadSound(fuelSwitchSound)) Console::printLine("Cannot load sound: ", fuelSwitchSound);
	if (!AudioManager::loadSound(ItemBeamCannon::laserSound)) Console::printLine("Cannot load sound: ", ItemBeamCannon::laserSound);
}
void InitShaders() {
	ShaderManager::load("lensShader", "../../assets/shaders/tetNormal.vs", "assets/lens.fs", "../../assets/shaders/tetNormal.gs");
}
$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	//Initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();

	ItemBeamCannon::rendererInit();

	InitBlueprints();

	InitRecipes();

	InitSounds();

	InitShaders();
}

// Idk whats that but lensShader doesnt work without it
$hook(void, StateGame, updateProjection, int width, int height)
{
	original(self, width, height);

	const Shader* sh = ShaderManager::get("lensShader");
	sh->use();
	glUniformMatrix4fv(glGetUniformLocation(sh->id(), "P"), 1, false, &self->projection3D[0][0]);
}

// Controls

void openBeamCannonInventory(GLFWwindow* window, int action, int mods)
{
	Player* player = &StateGame::instanceObj.player;
	if (player == nullptr || player->inventoryManager.isOpen()) return;

	ItemBeamCannon* beamCannon;
	beamCannon = dynamic_cast<ItemBeamCannon*>(player->hotbar.getSlot(player->hotbar.selectedIndex).get());
	if (!beamCannon) return;
	beamCannon->openInventory(player);
}

$hook(void,Player, mouseButtonInput, GLFWwindow* window, World* world, int button, int action, int mods) {
	ItemBeamCannon* beamCannon;
	beamCannon = dynamic_cast<ItemBeamCannon*>(self->hotbar.getSlot(self->hotbar.selectedIndex).get());
	if (!beamCannon) return original(self, window, world, button, action, mods);

	if (button == GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS && self->keys.shift) {
		beamCannon->openInventory(self);
	}
	else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
		beamCannon->isSelectedFuelDeadly = !beamCannon->isSelectedFuelDeadly;
		AudioManager::playSound4D(fuelSwitchSound, "ambience", self->cameraPos, self->vel);
	}
	else
		return original(self, window, world, button, action, mods);
}