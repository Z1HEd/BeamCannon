#pragma once
#include "hypercore.h"

using namespace fdm;

namespace hypercore {
	class EntityController {
	public:
		inline static void spawnEntity(World* world, std::unique_ptr<Entity>& entity) {
			static glm::vec4 entityPos;
			static Chunk* chunk;
			entityPos = entity->getPos();
			chunk = world->getChunkFromCoords(entityPos.x, entityPos.z, entityPos.w);
			if (chunk) world->addEntityToChunk(entity, chunk);
		}

		inline static void spawnEntityItem(World* world, std::unique_ptr<Item>& item, glm::vec4& position) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				item, position, {}
			);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, std::unique_ptr<Item>& item, glm::vec4& position, glm::vec4& velocity) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				item, position, velocity
			);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, const std::string& itemName, glm::vec4& position) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				Item::create(itemName, 1), position, {}
			);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, const std::string& itemName, glm::vec4& position, glm::vec4& velocity) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				Item::create(itemName, 1), position, velocity
			);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, const std::string& itemName, const int& count, glm::vec4& position) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				Item::create(itemName, count), position, {}
			);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, const std::string& itemName, const int& count, glm::vec4& position, glm::vec4& velocity) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				Item::create(itemName, count), position, velocity
			);
			spawnEntity(world, spawnedEntity);
		}
	};
}