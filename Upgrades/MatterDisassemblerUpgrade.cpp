#include "MatterDisassemblerUpgrade.h"

void MatterDisassemblerUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->diggingPower *= 3;
	beamCannon->entityDamage *= 0.75;
	beamCannon->fuelUsage *= 2;
}

bool MatterDisassemblerUpgrade::isCompatibleUpgrade(BeamCannonUpgrade* upgrade) {
	return true;
}

bool MatterDisassemblerUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<MatterDisassemblerUpgrade*>(other.get());
}

stl::string MatterDisassemblerUpgrade::getName() {
	return "Matter Disassembler Upgrade";
}

std::unique_ptr<Item> MatterDisassemblerUpgrade::clone() {
	auto result = std::make_unique<MatterDisassemblerUpgrade>();

	return result;
}
