#include "ConcentratorUpgrade.h"

void ConcentratorUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->effectiveDistance *= 4;
	beamCannon->entityDamage *= 0.75;
	beamCannon->diggingPower *= 0.75;
}

bool ConcentratorUpgrade::isCompatibleUpgrade(BeamCannonUpgrade* upgrade) {
	return true;
}

bool ConcentratorUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<ConcentratorUpgrade*>(other.get());
}

stl::string ConcentratorUpgrade::getName() {
	return "Concentrator Upgrade";
}

std::unique_ptr<Item> ConcentratorUpgrade::clone() {
	auto result = std::make_unique<ConcentratorUpgrade>();

	return result;
}
