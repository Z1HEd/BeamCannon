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
void OpticsUpgrade::render(const glm::ivec2& pos) {
	TexRenderer& tr = *ItemTool::tr; // or TexRenderer& tr = ItemTool::tr; after 4dmodding 2.2
	FontRenderer& fr = *ItemMaterial::fr;

	const Tex2D* ogTex = tr.texture; // remember the original texture

	static std::string iconPath = "";
	iconPath = std::format("{}{}.png", "assets/", getName().c_str());
	iconPath.erase(remove(iconPath.begin(), iconPath.end(), ' '), iconPath.end());

	tr.texture = ResourceManager::get(iconPath, true); // set to custom texture
	tr.setClip(0, 0, 36, 36);
	tr.setPos(pos.x, pos.y, 70, 72);
	tr.render();
	tr.texture = ogTex; // return to the original texture
}