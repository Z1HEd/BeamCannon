#pragma once

#include "hypercore.h"

using namespace fdm;

namespace hypercore {
	class RecipeController {
	public:
		static std::vector<nlohmann::json> recipes;
		static bool checkDuplicates;

		static void addRecipe(const std::string& resultName, int resultCount,
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

		static void enableDuplicateCheck() { checkDuplicates = true; }
		static void disableDuplicateCheck() { checkDuplicates = false; }
	};
	std::vector<nlohmann::json> RecipeController::recipes;
	bool RecipeController::checkDuplicates = false;

	// Add recipes
	$hookStatic(void, CraftingMenu, loadRecipes)
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
}