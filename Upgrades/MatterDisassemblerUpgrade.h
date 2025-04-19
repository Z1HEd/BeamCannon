#pragma once

#include "BeamCannonUpgrade.h"
#include <4dm.h>

using namespace fdm;

class MatterDisassemblerUpgrade : public BeamCannonUpgrade {
	void applyUpgrade(ItemBeamCannon* beamCannon) override;
	bool isCompatibleUpgrade(BeamCannonUpgrade* upgrade) override;

	bool isCompatible(const std::unique_ptr<Item>& other) override;
	stl::string getName() override;
	std::unique_ptr<Item> clone() override;
};