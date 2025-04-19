#include "MagnetUpgrade.h"

void MagnetUpgrade::applyUpgrade(ItemBeamCannon* beamCannon) {
	beamCannon->hasMagnetEffect = true;
}

bool MagnetUpgrade::isCompatibleUpgrade(BeamCannonUpgrade* upgrade) {
	return dynamic_cast<MagnetUpgrade*>(upgrade) == nullptr; // Can only apply once
}

bool MagnetUpgrade::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<MagnetUpgrade*>(other.get());
}

stl::string MagnetUpgrade::getName() {
	return "Magnet Upgrade";
}

std::unique_ptr<Item> MagnetUpgrade::clone() {
	auto result = std::make_unique<MagnetUpgrade>();

	return result;
}
