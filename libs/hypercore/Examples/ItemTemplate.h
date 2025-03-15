#pragma once

#include <4dm.h>

using namespace fdm;

class ItemTemplate : public Item {
public:

	// Virtual functions overrides
	bool isCompatible(const std::unique_ptr<Item>& other) override;
	stl::string getName() override;
	bool isDeadly() override;
	uint32_t getStackLimit() override;
	bool action(World* world, Player* player, int action) override;
	void renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) override;
	std::unique_ptr<Item> clone() override;
	nlohmann::json saveAttributes() override;
	
};