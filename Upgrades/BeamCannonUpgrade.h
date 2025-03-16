#pragma once

#include <4dm.h>
#include "../ItemBeamCannon.h"

using namespace fdm;

class BeamCannonUpgrade : public Item {
public:
	//Virtual functions
	virtual void applyUpgrade(ItemBeamCannon* beamCannon) {};

	// Virtual functions overrides
	bool isDeadly() override;
	uint32_t getStackLimit() override;
	bool action(World* world, Player* player, int action) override;
	void renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) override;

	nlohmann::json saveAttributes() override;

};