#include "OpticsUpgrade.h"

void OpticsUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->hasGlassesEffect = true;
}

bool OpticsUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<OpticsUpgrade*>(other.get());
}

stl::string OpticsUpgrade::getName() {
	return "4D Optics Upgrade";
}

std::unique_ptr<Item> OpticsUpgrade::clone() {
	auto result = std::make_unique<OpticsUpgrade>();

	return result;
}