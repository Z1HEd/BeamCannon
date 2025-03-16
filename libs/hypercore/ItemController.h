#pragma once

#include "hypercore.h"

using namespace fdm;

namespace hypercore {
	class ItemController {
	public:
		struct ItemData {
			std::string name;
			std::string type;
			std::string iconPath;
			nlohmann::json attributes;
			bool isDeadly;
		};

		inline static std::vector<ItemController::ItemData> items = {};
		inline static std::string defaultIconFolder = "";

		inline static void setDefaultIconFolder(const std::string& path) {
			defaultIconFolder = path;
		}
		
		// Assumes item is material, and path is defaultIconFolder+itemName.png without spaces
		inline static void addMaterial(const std::string& itemName, const bool& isDeadly = false) {
			addItem(itemName, "material", isDeadly);
		}
		
		// Assumes path is defaultIconFolder+itemName.png without spaces
		inline static void addItem(const std::string& itemName, const std::string& itemType, const bool& isDeadly=false) {
			
			static std::string iconPath="";

			iconPath= std::format("{}{}.png", defaultIconFolder, itemName.c_str());
			iconPath.erase(remove(iconPath.begin(), iconPath.end(), ' '), iconPath.end());

			addItem(itemName, itemType,iconPath, isDeadly);
		}
		
		// Adds item to the game.
		inline static void addItem(const std::string& itemName, const std::string& itemType, const std::string& iconPath, const bool& isDeadly=false) {
			addItemWithAttributes(itemName, itemType, iconPath,nlohmann::json::object(), isDeadly);
		}
		// Assumes path is defaultIconFolder+itemName.png without spaces
		inline static void addItemWithAttributes(const std::string& itemName, const std::string& itemType,const nlohmann::json& attributes, const bool& isDeadly = false) {

			static std::string iconPath = "";

			iconPath = std::format("{}{}.png", defaultIconFolder, itemName.c_str());
			iconPath.erase(remove(iconPath.begin(), iconPath.end(), ' '), iconPath.end());

			addItemWithAttributes(itemName, itemType, iconPath, attributes, isDeadly);
		}

		// Adds item to the game.
		inline static void addItemWithAttributes(const std::string& itemName, const std::string& itemType, const std::string& iconPath,const nlohmann::json& attributes, const bool& isDeadly = false) {
			if (Item::blueprints->contains(itemName)) return;
			items.push_back(ItemData{ itemName,itemType,iconPath,attributes, isDeadly });
		}

		// Initialises item blueprints - called automatically.
		inline static void initBlueprints() {
			for (int i = 0;i < items.size(); i++)
				(*Item::blueprints)[items[i].name] =
			{
				{ "type", items[i].type },
				{ "baseAttributes", items[i].attributes}
			};
		}
	};
}