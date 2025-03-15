#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include "libs/hypercore/hypercore.h"
#include "ItemBeamCannon.h"

using namespace hypercore;

std::string chippingSoundFail = "assets/ChippingFail.ogg";
std::string chippingSoundSuccess= "assets/ChippingSuccess.ogg";

// Lens Chipping mechanic
$hook(void,Player, mouseButtonInput,GLFWwindow* window, World* world, int button, int action, int mods) {
	
	if (button !=GLFW_MOUSE_BUTTON_2 || action !=GLFW_PRESS) return original(self, window, world, button, action, mods);

	
	if (!(self->equipment.getSlot(0)->get() != nullptr && self->equipment.getSlot(0)->get()->getName() == "Rock")
		&& !(self->hotbar.getSlot(self->hotbar.selectedIndex)->get() != nullptr && self->hotbar.getSlot(self->hotbar.selectedIndex)->get()->getName() == "Rock"))
		return original(self, window, world, button, action, mods);
	std::unique_ptr<Item>* lens = nullptr;
	
	if (self->equipment.getSlot(0)->get() != nullptr && (
		self->equipment.getSlot(0)->get()->getName() == "Red Lens" ||
		self->equipment.getSlot(0)->get()->getName() == "Green Lens" ||
		self->equipment.getSlot(0)->get()->getName() == "Blue Lens")
		) lens = self->equipment.getSlot(0);

	else if (self->hotbar.getSlot(self->hotbar.selectedIndex)->get() != nullptr && (
		self->hotbar.getSlot(self->hotbar.selectedIndex)->get()->getName() == "Red Lens" ||
		self->hotbar.getSlot(self->hotbar.selectedIndex)->get()->getName() == "Green Lens" ||
		self->hotbar.getSlot(self->hotbar.selectedIndex)->get()->getName() == "Blue Lens")
		) lens = self->hotbar.getSlot(self->hotbar.selectedIndex);

	if (lens ==nullptr) return original(self, window, world, button, action, mods);

	lens->get()->count--;
	if (lens->get()->count == 0) lens->release();

	if (rand() % 3 != 0) {
		AudioManager::playSound4D(chippingSoundFail, "ambience", self->cameraPos, self->vel);
		return;
	}

	AudioManager::playSound4D(chippingSoundSuccess, "ambience", self->cameraPos, self->vel);

	Chunk* chunk = world->getChunkFromCoords(self->pos.x, self->pos.z, self->pos.w);
	std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
		Item::create(stl::string("Flawless ") + lens->get()->getName(), 1), self->cameraPos, self->vel
	);
	world->addEntityToChunk(spawnedEntity,chunk);
}
// Initialise stuff
void gameInit() {

	// Add custom items to the game
	ItemManager::setDefaultIconFolder("assets/");

	ItemManager::addItem("Flawless Red Lens");
	ItemManager::addItem("Flawless Green Lens");
	ItemManager::addItem("Flawless Blue Lens");

	ItemManager::addItem("Deadly Casing");
	ItemManager::addItem("Iron Plate");
	ItemManager::addItem("Solenoid Wire");

	ItemManager::addItem("Beam Concentrator");
	ItemManager::addItem("Beam Cannon","beamCannon");

	// Add Recipes
	RecipeManager::addRecipe("Deadly Casing", 1, { {"Deadly Bars",4}});
	RecipeManager::addRecipe("Iron Plate", 1, { {"Iron Bars",3} });
	RecipeManager::addRecipe("Solenoid Wire", 1, { {"Solenoid Bars",2} });

	RecipeManager::addRecipe("Beam Concentrator", 1, { {"Flawless Blue Lens",1},{"Flawless Red Lens",1},{"Flawless Green Lens",1},{"Iron Plate",1} });
	RecipeManager::addRecipe("Beam Cannon", 1, { {"Beam Concentrator",1},{"Deadly Casing",2},{"Iron Plate",2},{"Solenoid Wire",3} });

	// Load sounds
	SoundLoader::addSound(chippingSoundFail);
	SoundLoader::addSound(chippingSoundSuccess);
}