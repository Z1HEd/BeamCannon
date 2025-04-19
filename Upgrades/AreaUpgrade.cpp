#include "AreaUpgrade.h"

void AreaUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->diggingArea += 1;
	beamCannon->diggingPower /= 3.5;
}

bool AreaUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<AreaUpgrade*>(other.get());
}
bool AreaUpgrade::isCompatibleUpgrade(BeamCannonUpgrade* upgrade) {
	return true;
}

stl::string AreaUpgrade::getName() {
	return "Area Upgrade";
}

std::unique_ptr<Item> AreaUpgrade::clone() {
	auto result = std::make_unique<AreaUpgrade>();

	return result;
}