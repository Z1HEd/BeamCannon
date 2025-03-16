//#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.
#include "hypercore.h"
#include <4dm.h>

using namespace fdm;
using namespace hypercore;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD _reason, LPVOID lpReserved)
{
	if (_reason == DLL_PROCESS_ATTACH && HyperCore::startConsoleFlag)
		fdm::startConsole();
	return TRUE;
}

$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	//Initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();

	gameInit();

	ItemController::initBlueprints();

	SoundController::loadSounds();
}


// Render item icons
$hookStatic(void, Item, renderItem,std::unique_ptr<Item>& item, const glm::ivec2& pos)
{
	auto index = std::find_if(ItemController::items.begin(), ItemController::items.end(), [&](const ItemController::ItemData& itemData) {
		return itemData.name == item->getName();
		}) - ItemController::items.begin();

	if (index == ItemController::items.size())
		return original(item, pos);

	TexRenderer& tr = *ItemTool::tr; // or TexRenderer& tr = ItemTool::tr; after 0.3
	FontRenderer& fr = *ItemMaterial::fr;

	const Tex2D* ogTex = tr.texture; // remember the original texture

	tr.texture = ResourceManager::get(ItemController::items[index].iconPath, true); // set to custom texture
	tr.setClip(0, 0, 36, 36);
	tr.setPos(pos.x, pos.y, 70, 72);
	tr.render();
	if (item->getStackLimit()>1) {
		fr.setText(std::to_string(item->count));
		fr.pos = pos + glm::ivec2{ 40,40 };
		fr.updateModel();
		fr.render();
	}

	tr.texture = ogTex; // return to the original texture

}

//Deadly text effect
$hook(bool, ItemMaterial, isDeadly)
{
	auto index = std::find_if(ItemController::items.begin(), ItemController::items.end(), [&](const ItemController::ItemData& item) {
		return item.name == self->getName();
		}) - ItemController::items.begin();

	if (index == ItemController::items.size())
		return original(self);

	return ItemController::items[index].isDeadly;
}

// Add recipes
inline $hookStatic(void, CraftingMenu, loadRecipes)
{
	static bool recipesLoaded = false;

	if (recipesLoaded) return;

	recipesLoaded = true;

	original();

	if (RecipeController::recipes.empty()) return;

	if (RecipeController::checkDuplicates)
		for (const auto& recipe : RecipeController::recipes) {
			if (std::any_of(CraftingMenu::recipes->begin(),
				CraftingMenu::recipes->end(),
				[&recipe](const nlohmann::json& globalRecipe) {
					return globalRecipe == recipe;
				})) continue;
			CraftingMenu::recipes->push_back(recipe);
		}
	else
		for (const auto& recipe : RecipeController::recipes)
			CraftingMenu::recipes->push_back(recipe);
}