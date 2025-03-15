#pragma once

#include "hypercore.h"

using namespace fdm;

namespace hypercore {
	class RecipeManager {
	public:
		static std::vector<nlohmann::json> recipes;

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
	};
    std::vector<nlohmann::json> RecipeManager::recipes;

    // Add recipes
    $hookStatic(void, CraftingMenu, loadRecipes)
    {
        static bool recipesLoaded = false;

        if (recipesLoaded) return;

        recipesLoaded = true;

        original();

        if (RecipeManager::recipes.empty()) return;

        for (const auto& recipe : RecipeManager::recipes) {
            if (std::any_of(CraftingMenu::recipes->begin(),
                CraftingMenu::recipes->end(),
                [&recipe](const nlohmann::json& globalRecipe) {
                    return globalRecipe == recipe;
                })) continue;
            CraftingMenu::recipes->push_back(recipe);
        }
        
    }
}