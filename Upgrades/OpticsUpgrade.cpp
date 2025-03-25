#include "OpticsUpgrade.h"

void OpticsUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->hasGlassesEffect = true;
}

bool OpticsUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<OpticsUpgrade*>(other.get());
}
bool OpticsUpgrade::isCompatibleUpgrade(BeamCannonUpgrade* upgrade) {
	return dynamic_cast<OpticsUpgrade*>(upgrade) == nullptr; // Can only apply once
}

stl::string OpticsUpgrade::getName() {
	return "4D Optics Upgrade";
}

std::unique_ptr<Item> OpticsUpgrade::clone() {
	auto result = std::make_unique<OpticsUpgrade>();

	return result;
}