#pragma once

#include "hypercore.h"

using namespace fdm;

namespace hypercore {
	class RecipeController {
	public:
		inline static  std::vector<nlohmann::json> recipes = {};
		inline static bool checkDuplicates = false;

		inline static void addRecipe(const std::string& resultName, int resultCount,
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

		inline static void enableDuplicateCheck() { checkDuplicates = true; }
		inline static void disableDuplicateCheck() { checkDuplicates = false; }
	};
}