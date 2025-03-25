#pragma once

#include <4dm.h>
#include "../ItemBeamCannon.h"

using namespace fdm;

class ItemBeamCannon;
class BeamCannonUpgrade : public Item {
public:
	//Virtual functions
	virtual void applyUpgrade(ItemBeamCannon* beamCannon) {};
	virtual bool isCompatibleUpgrade(BeamCannonUpgrade* upgrade) { return true; };
	bool canBePutInto(InventoryGrid* inventory);

	// Virtual functions overrides
	bool isDeadly() override;
	uint32_t getStackLimit() override;
	void render(const glm::ivec2& pos) override;
	void renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) override;

	nlohmann::json saveAttributes() override;

};