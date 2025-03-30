#include "GyroscopeUpgrade.h"

void GyroscopeUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->hasCompassEffect = true;
}

bool GyroscopeUpgrade::isCompatibleUpgrade(BeamCannonUpgrade* upgrade) {
	return dynamic_cast<GyroscopeUpgrade*>(upgrade) == nullptr; // Can only apply once
}

bool GyroscopeUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<GyroscopeUpgrade*>(other.get());
}

stl::string GyroscopeUpgrade::getName() {
	return "Gyroscope Upgrade";
}

std::unique_ptr<Item> GyroscopeUpgrade::clone() {
	auto result = std::make_unique<GyroscopeUpgrade>();

	return result;
}
