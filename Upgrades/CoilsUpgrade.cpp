#include "CoilsUpgrade.h"

void CoilsUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->entityDamage *= 2;
	beamCannon->fuelUsage *= 1.5;
	beamCannon->effectiveDistance *= 0.75;
}

bool CoilsUpgrade::isCompatibleUpgrade(BeamCannonUpgrade* upgrade) {
	return true;
}

bool CoilsUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<CoilsUpgrade*>(other.get());
}

stl::string CoilsUpgrade::getName() {
	return "Coils Upgrade";
}

std::unique_ptr<Item> CoilsUpgrade::clone() {
	auto result = std::make_unique<CoilsUpgrade>();

	return result;
}
