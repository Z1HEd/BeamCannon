#include "FiltrationUpgrade.h"

void FiltrationUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->fuelUsage /= 3;
}

bool FiltrationUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<FiltrationUpgrade*>(other.get());
}
bool FiltrationUpgrade::isCompatibleUpgrade(BeamCannonUpgrade* upgrade) {
	return true;
}

stl::string FiltrationUpgrade::getName() {
	return "Filtration Upgrade";
}

std::unique_ptr<Item> FiltrationUpgrade::clone() {
	auto result = std::make_unique<FiltrationUpgrade>();

	return result;
}