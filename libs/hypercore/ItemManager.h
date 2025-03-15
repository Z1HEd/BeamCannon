#pragma once

#include "hypercore.h"

using namespace fdm;

namespace hypercore {
	class ItemManager {
	public:
		struct ItemData {
			std::string name;
			std::string type;
			std::string iconPath;
			bool isDeadly;
		};
		static std::vector<ItemData> items;
		static std::string defaultIconFolder;

		static void setDefaultIconFolder(const std::string& path) {
			defaultIconFolder = path;
		}
		
		// Assumes item is material, and path is defaultIconFolder+itemName.png without spaces
		static void addItem(const std::string& itemName, const bool& isDeadly = false) {
			addItem(itemName, "material", isDeadly);
		}
		
		// Assumes path is defaultIconFolder+itemName.png without spaces
		static void addItem(const std::string& itemName, const std::string& itemType, const bool& isDeadly=false) {
			
			static std::string iconPath="";

			iconPath= std::format("{}{}.png", defaultIconFolder, itemName.c_str());
			iconPath.erase(remove(iconPath.begin(), iconPath.end(), ' '), iconPath.end());

			addItem(itemName, "material",iconPath, isDeadly);
		}
		
		// Adds item to the game.
		static void addItem(const std::string& itemName, const std::string& itemType, const std::string& iconPath, const bool& isDeadly=false) {
			if (Item::blueprints->contains(itemName)) return;
			items.push_back(ItemData{ itemName,itemType,iconPath, isDeadly});
		}
		// Initialises item blueprints - called automatically.
		static void initBlueprints() {
			for (int i = 0;i < items.size(); i++)
				(*Item::blueprints)[items[i].name] =
			{
				{ "type", items[i].type },
				{ "baseAttributes", nlohmann::json::object() } // no attributes
			};
		}
	};
	std::vector<ItemManager::ItemData> ItemManager::items = {};
	std::string ItemManager::defaultIconFolder = "";


	// Render item icons
	$hook(void, ItemMaterial, render, const glm::ivec2& pos) 
	{
		auto index = std::find_if(ItemManager::items.begin(), ItemManager::items.end(), [&](const ItemManager::ItemData& item) {
			return item.name == self->getName();
			}) - ItemManager::items.begin();

		if (index == ItemManager::items.size())
			return original(self, pos);

		TexRenderer& tr = *ItemTool::tr; // or TexRenderer& tr = ItemTool::tr; after 0.3
		const Tex2D* ogTex = tr.texture; // remember the original texture

		tr.texture = ResourceManager::get(ItemManager::items[index].iconPath, true); // set to custom texture
		tr.setClip(0, 0, 36, 36);
		tr.setPos(pos.x, pos.y, 70, 72);
		tr.render();

		tr.texture = ogTex; // return to the original texture

	}

	//Deadly text effect
	$hook(bool, ItemMaterial, isDeadly)
	{
		auto index = std::find_if(ItemManager::items.begin(), ItemManager::items.end(), [&](const ItemManager::ItemData& item) {
			return item.name == self->getName();
			}) - ItemManager::items.begin();

		if (index == ItemManager::items.size())
			return original(self);

		return ItemManager::items[index].isDeadly;
	}
}