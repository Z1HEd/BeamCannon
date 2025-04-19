#pragma once
#include <4dm.h>

using namespace fdm;

namespace hypercore {
	class EntityController {
	public:
		inline static void spawnEntity(World* world, std::unique_ptr<Entity>& entity) {
			glm::vec4 entityPos = entity->getPos();
			Chunk*  chunk = world->getChunkFromCoords(entityPos.x, entityPos.z, entityPos.w);
			if (chunk) world->addEntityToChunk(entity, chunk);
		}

		inline static void spawnEntityItem(World* world, std::unique_ptr<Item> item, const  glm::vec4& position) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				std::move(item), position, {}
			);
			((EntityItem*)spawnedEntity.get())->combineWithNearby(world);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, std::unique_ptr<Item> item, const  glm::vec4& position, const  glm::vec4& velocity) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				std::move(item), position, velocity
			);
			((EntityItem*)spawnedEntity.get())->combineWithNearby(world);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, const std::string& itemName,const glm::vec4& position) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				Item::create(itemName, 1), position, {}
			);
			((EntityItem*)spawnedEntity.get())->combineWithNearby(world);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, const std::string& itemName, const  glm::vec4& position, const  glm::vec4& velocity) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				Item::create(itemName, 1), position, velocity
			);
			((EntityItem*)spawnedEntity.get())->combineWithNearby(world);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, const std::string& itemName, const int& count, const  glm::vec4& position) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				Item::create(itemName, count), position, {}
			);
			((EntityItem*)spawnedEntity.get())->combineWithNearby(world);
			spawnEntity(world, spawnedEntity);
		}

		inline static void spawnEntityItem(World* world, const std::string& itemName, const int& count, const  glm::vec4& position, const  glm::vec4& velocity) {
			std::unique_ptr<Entity> spawnedEntity = EntityItem::createWithItem(
				Item::create(itemName, count), position, velocity
			);
			((EntityItem*)spawnedEntity.get())->combineWithNearby(world);
			spawnEntity(world, spawnedEntity);
		}
	};
}