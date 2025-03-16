#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include "libs/hypercore/hypercore.h"
#include "ItemBeamCannon.h"

using namespace hypercore;

std::string chippingSoundFail = "assets/ChippingFail.ogg";
std::string chippingSoundSuccess= "assets/ChippingSuccess.ogg";

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

	if (button != GLFW_MOUSE_BUTTON_2 || action != GLFW_PRESS) return original(self, window, world, button, action, mods);

	std::unique_ptr<Item>& mainHandItem = PlayerController::getMainHandItem(self);
	std::unique_ptr<Item>& offHandItem = PlayerController::getOffHandItem(self);

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

// Initialise stuff
void gameInit() {

	// Add items
	ItemController::setDefaultIconFolder("assets/");

	ItemController::addItem("Flawless Red Lens");
	ItemController::addItem("Flawless Green Lens");
	ItemController::addItem("Flawless Blue Lens");

	ItemController::addItem("Deadly Casing");
	ItemController::addItem("Iron Plate");
	ItemController::addItem("Solenoid Wire");

	ItemController::addItem("Beam Concentrator");
	ItemController::addItem("Beam Cannon","beamCannon");

	// Add Recipes
	RecipeController::enableDuplicateCheck();

	RecipeController::addRecipe("Deadly Casing", 1, { {"Deadly Bars",4}});
	RecipeController::addRecipe("Iron Plate", 1, { {"Iron Bars",3} });
	RecipeController::addRecipe("Solenoid Wire", 1, { {"Solenoid Bars",2} });

	RecipeController::addRecipe("Beam Concentrator", 1, { {"Flawless Blue Lens",1},{"Flawless Red Lens",1},{"Flawless Green Lens",1},{"Iron Plate",1} });
	RecipeController::addRecipe("Beam Cannon", 1, { {"Beam Concentrator",1},{"Deadly Casing",2},{"Iron Plate",2},{"Solenoid Wire",3} });

	// Load sounds
	SoundController::addSound(chippingSoundFail);
	SoundController::addSound(chippingSoundSuccess);
}