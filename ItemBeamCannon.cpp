#include "ItemBeamCannon.h"
#include "Upgrades/BeamCannonUpgrade.h"
#include "EntityController.h"

using namespace fdm;
using namespace hypercore;

MeshRenderer ItemBeamCannon::renderer = {};
std::string ItemBeamCannon::fuelFlushSound = "";
//TODO: when 4D modding 2.2 adds SoLoud headers, make a proper sound for lazer
std::string ItemBeamCannon::laserSound = "";


bool ItemBeamCannon::isCompatible(const std::unique_ptr<Item>& other)
{
	return dynamic_cast<ItemBeamCannon*>(other.get());
}

stl::string ItemBeamCannon::getName() {
	return "Beam Cannon";
}

bool ItemBeamCannon::isDeadly() {
	return true; // Used for fancy text effect 
}

uint32_t ItemBeamCannon::getStackLimit() {
	return 1; 
}

void ItemBeamCannon::resetUpgrades() {
	hasGlassesEffect = false;
	hasCompassEffect = false;
	effectiveDistance = 15;
	entityDamage = 10;
	diggingPower = 3;
	fuelUsage = 0.01f;
	effectArea = 0.0f;
}
void ItemBeamCannon::applyUpgrades() {
	for (unsigned int i = 0;i < inventory.getSlotCount(); i++) {
		if (auto* upgrade = dynamic_cast<BeamCannonUpgrade*>(inventory.getSlot(i)->get()))
			upgrade->applyUpgrade(this);
	}
}
void ItemBeamCannon::reloadUpgrades() {
	resetUpgrades();
	applyUpgrades();
}

int ItemBeamCannon::getSelectedFuelCount(InventoryPlayer& inventory) {
	int count = 0;
	for (unsigned int slot = 0;slot < inventory.getSlotCount(); slot++) {
		Item* i = inventory.getSlot(slot)->get();
		if (i != nullptr && i->getName() == (isSelectedFuelDeadly ? "Deadly Fuel" : "Biofuel"))
			count += i->count;

	}
	return count;
}
void ItemBeamCannon::consumeSelectedFuel(InventoryPlayer& inventory) {
	for (unsigned int slot = 0;slot < inventory.getSlotCount(); slot++) {
		Item* i = inventory.getSlot(slot)->get();
		if (i != nullptr && i->getName() == (isSelectedFuelDeadly ? "Deadly Fuel" : "Biofuel")) {
			i->count--;
			if (i->count < 1) inventory.getSlot(slot)->reset();
			fuelLevel = 1.0f;
			isFuelDeadly = isSelectedFuelDeadly;
			return;
		}
	}
}
void ItemBeamCannon::rendererInit() {
	MeshBuilder mesh{ BlockInfo::HYPERCUBE_FULL_INDEX_COUNT };
	// vertex position attribute
	mesh.addBuff(BlockInfo::hypercube_full_verts, sizeof(BlockInfo::hypercube_full_verts));
	mesh.addAttr(GL_UNSIGNED_BYTE, 4, sizeof(glm::u8vec4));
	// per-cell normal attribute
	mesh.addBuff(BlockInfo::hypercube_full_normals, sizeof(BlockInfo::hypercube_full_normals));
	mesh.addAttr(GL_FLOAT, 1, sizeof(GLfloat));

	mesh.setIndexBuff(BlockInfo::hypercube_full_indices, sizeof(BlockInfo::hypercube_full_indices));

	renderer.setMesh(&mesh);
}

static void destroyBlock(World* world,const glm::ivec4& blockPos) {
	EntityController::spawnEntityItem(world,BlockInfo::blockNames->find(world->getBlock(blockPos))->second, glm::vec4 { 0.5 }+ glm::vec4(blockPos));
	world->setBlockUpdate(blockPos, BlockInfo::TYPE::AIR);
}

void ItemBeamCannon::tryShooting(Player* player,double dt) {
	static float timeSinceLastLazerSound = 0;

	static double timeSinceLastTick = 0;
	constexpr double timeBetweenTicks = 0.05; // 20 ticks per second
	timeSinceLastTick += dt;
	timeSinceLastLazerSound += dt;

	if (timeSinceLastLazerSound >= 0.5f) {
		timeSinceLastLazerSound = 0;
		AudioManager::playSound4D(laserSound, "ambience", player->cameraPos, player->vel);
	}

	if (timeSinceLastTick < timeBetweenTicks) return;

	if (isFlushing && fuelLevel > 0.0f) {
		AudioManager::playSound4D(fuelFlushSound, "ambience", player->cameraPos, player->vel);
		fuelLevel = 0.0f;
	}

	if (fuelLevel <= 0.0f && player->touchingGround && !isFlushing) consumeSelectedFuel(player->inventoryAndEquipment);

	//fuelLevel -= fuelUsage;

	timeSinceLastTick = 0;

	glm::vec4 reachStartpoint = player->cameraPos;
	glm::vec4 reachEndpoint = player->cameraPos + player->forward * effectiveDistance;

	Entity* intersect = StateGame::instanceObj->world->getEntityIntersection(reachStartpoint, reachEndpoint, player->EntityPlayerID);

	glm::ivec4 blockPos = reachStartpoint;
	glm::ivec4 endPos;
	static glm::ivec4 prevPos = {};
	static float destroyProgress = 0;
	if (intersect != nullptr) {
		intersect->takeDamage(entityDamage, StateGame::instanceObj->world.get());
		return;
	}
	else if (StateGame::instanceObj->world->castRay(reachStartpoint, blockPos, endPos, reachEndpoint)) {
		if (prevPos != endPos)
			destroyProgress = 0;
		destroyProgress += diggingPower / 20.0;
		if (destroyProgress >= 1) {
			destroyBlock(StateGame::instanceObj->world.get(), endPos);
			destroyProgress = 0;
		}
		prevPos = endPos;
	}

}

void ItemBeamCannon::update(Player* player,double dt) {
	if (player->keys.rightMouseDown)
		tryShooting(player, dt);
}

$hook(void, Player, update, World* world, double _, EntityPlayer* entityPlayer) { // dt is useless bcs its hardcoded to 0.01
	original(self, world, _, entityPlayer);

	ItemBeamCannon* cannon;
	cannon = dynamic_cast<ItemBeamCannon*>(self->hotbar.getSlot(self->hotbar.selectedIndex)->get());
	if (!cannon) cannon = dynamic_cast<ItemBeamCannon*>(self->equipment.getSlot(0)->get());
	if (!cannon) return;

	static double lastTime = glfwGetTime() - 0.01;
	double curTime = glfwGetTime();
	double dt = curTime - lastTime;
	lastTime = curTime;

	cannon->update(self, dt);
}

void ItemBeamCannon::openInventory(Player* player) {
	if (player->inventoryManager.isOpen()) return;
	player->inventoryManager.primary = &player->playerInventory;
	player->shouldResetMouse = true;
	player->inventoryManager.secondary = &inventory;


	player->inventoryManager.craftingMenu.updateAvailableRecipes();
	player->inventoryManager.updateCraftingMenuBox();

	inventorySession.inventory = &inventory;
	inventorySession.manager = &player->inventoryManager;
}

void ItemBeamCannon::render(const glm::ivec2& pos) {
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

float ItemBeamCannon::getLaserLength() {
	Player* player = &StateGame::instanceObj->player;

	glm::vec4 reachStartpoint = player->cameraPos;
	glm::vec4 reachEndpoint = player->cameraPos + player->forward * effectiveDistance;

	Entity* intersect = StateGame::instanceObj->world->getEntityIntersection(reachStartpoint, reachEndpoint, player->EntityPlayerID);

	glm::ivec4 blockPos = reachStartpoint;
	glm::ivec4 endPos;

	if (intersect != nullptr) {
		reachEndpoint = intersect->getPos();
		return glm::length(reachEndpoint - reachStartpoint);
	}
	else {
		StateGame::instanceObj->world->castRay(reachStartpoint, blockPos, endPos, reachEndpoint);
		return effectiveDistance - glm::length(reachEndpoint - reachStartpoint);
	}
	
}

void ItemBeamCannon::renderEntity(const m4::Mat5& MV, bool inHand, const glm::vec4& lightDir) {
	
	Player* player = &StateGame::instanceObj->player;

	static double lastTime = glfwGetTime() - 0.01;
	double curTime = glfwGetTime();
	double dt = curTime - lastTime;
	lastTime = curTime;

	glm::vec4 fuelColor;
	if (isFuelDeadly)
		fuelColor = glm::vec4{ 147.0f / 255.0f,55.0f / 255.0f,118.0f / 255.0f,0.75f };
	else
		fuelColor = glm::vec4{ 82.0f / 255.0f,144.0f / 255.0f,40.0f / 255.0f,0.75f };

	// =========STATIC PART==========

	m4::Mat5 handleMat = MV;
	handleMat.translate(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.001f });
	handleMat *= m4::Rotor
	(
		{
			m4::wedge({0, 0, 1, 0}, {0, 1, 0, 0}), // ZY
			-glm::pi<float>() / 6
		}
	);
	handleMat.scale(glm::vec4{ 0.2f,1.f,0.25f,0.25f });
	handleMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 slopeMat = MV;
	slopeMat.translate(glm::vec4{ 0.0f, .8f, -0.5f, 0.001f });
	slopeMat *= m4::Rotor
	(
		{
			m4::wedge({0, 0, 1, 0}, {0, 1, 0, 0}), // ZY
			-glm::pi<float>() / 4
		}
	);
	slopeMat.scale(glm::vec4{ 0.4f,.7f,0.5f,0.4f });
	slopeMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 lowerMat = MV;
	lowerMat.translate(glm::vec4{ 0.0f, .5f, .0f, 0.001f });
	lowerMat.scale(glm::vec4{ 0.4f,.5f,3.5f,0.4f });
	lowerMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 upperMat = MV;
	upperMat.translate(glm::vec4{ 0.0f, 0.978f, -1.16f, 0.001f });
	upperMat.scale(glm::vec4{ 0.4f,.48f,1.195f,0.4f });
	upperMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 buttMat = MV;
	buttMat.translate(glm::vec4{ 0.0f, 0.0f, 1.75f, 0.001f });
	buttMat.scale(glm::vec4{ 0.4f,1.5f,0.4f,0.4f });
	buttMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 button1Mat = MV;
	button1Mat.translate(glm::vec4{ 0.0f, .8f, -0.5f, 0.001f });
	button1Mat *= m4::Rotor
	(
		{
			m4::wedge({0, 0, 1, 0}, {0, 1, 0, 0}), // ZY
			-glm::pi<float>() / 4
		}
	);
	button1Mat.translate(glm::vec4{ 0.1f , -0.23f, 0.25f, 0.0f });
	button1Mat.scale(glm::vec4{ 0.1f,.1f,0.05f,0.13f });
	button1Mat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });


	m4::Mat5 button2Mat = MV;
	button2Mat.translate(glm::vec4{ 0.0f, .8f, -0.5f, 0.001f });
	button2Mat *= m4::Rotor
	(
		{
			m4::wedge({0, 0, 1, 0}, {0, 1, 0, 0}), // ZY
			-glm::pi<float>() / 4
		}
	);
	button2Mat.translate(glm::vec4{ -0.1f , -0.23f, 0.25f, 0.0f });
	button2Mat.scale(glm::vec4{ 0.1f,.1f,0.05f,0.13f });
	button2Mat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 fuelMat = MV;
	fuelMat.translate(glm::vec4{ 0.0f, .8f, -0.5f, 0.001f });
	fuelMat *= m4::Rotor
	(
		{
			m4::wedge({0, 0, 1, 0}, {0, 1, 0, 0}), // ZY
			-glm::pi<float>() / 4
		}
	);
	fuelMat.translate(glm::vec4{ 0.0f , -0.15f + 0.15f * fuelLevel, 0.250f, 0.0f });
	fuelMat.scale(glm::vec4{ 0.3f,.30 * fuelLevel,0.031f,0.07f });
	fuelMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 fuelBackgroundMat = MV;
	fuelBackgroundMat.translate(glm::vec4{ 0.0f, .8f, -0.5f, 0.001f });
	fuelBackgroundMat *= m4::Rotor
	(
		{
			m4::wedge({0, 0, 1, 0}, {0, 1, 0, 0}), // ZY
			-glm::pi<float>() / 4
		}
	);
	fuelBackgroundMat.translate(glm::vec4{ 0.0f , 0.0f, 0.249f, 0.0f });
	fuelBackgroundMat.scale(glm::vec4{ 0.3f,.30f,0.03f,0.07f });
	fuelBackgroundMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 lensRedMat = MV;
	lensRedMat.translate(glm::vec4{ 0.0f, 0.73f, -3.4f, 0.0001f });
	lensRedMat.scale(glm::vec4{ 0.12f,0.12f,0.03f,0.12f });

	m4::Mat5 lensGreenMat = MV;
	lensGreenMat.translate(glm::vec4{ 0.0f, 0.73f, -2.8f, 0.0001f });
	lensGreenMat.scale(glm::vec4{ 0.12f,0.12f,0.03f,0.12f });

	m4::Mat5 lensBlueMat = MV;
	lensBlueMat.translate(glm::vec4{ 0.0f, 0.73f, -2.2f, 0.0001f });
	lensBlueMat.scale(glm::vec4{ 0.12f,0.12f,0.03f,0.12f });

	m4::Mat5 upperLensHolderMat = MV;
	upperLensHolderMat.translate(glm::vec4{ 0.0f, 1.15f, -2.6f, 0.001f });
	upperLensHolderMat.scale(glm::vec4{ 0.1f,.1f,1.8f,0.1f });
	upperLensHolderMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	m4::Mat5 lowerLensHolderMat = MV;
	lowerLensHolderMat.translate(glm::vec4{ 0.0f, 0.3f, -2.6f, 0.001f });
	lowerLensHolderMat.scale(glm::vec4{ 0.1f,.1f,1.8f,0.1f });
	lowerLensHolderMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });

	static float laserRot = 0;
	float laserLength = std::max(getLaserLength()*7.5,3.0);
	laserRot += dt;

	m4::Mat5 laserMat = MV;
	laserMat.translate(glm::vec4{ 0.0f, 0.73f, -2.2f - laserLength/2, 0.0001f });
	laserMat.scale(glm::vec4{ 0.2f,.2f,laserLength,0.2f});
	laserMat *= m4::Rotor
	(
		{
			m4::wedge({1, 0, 0, 0}, {0, 1, 0, 0}), // XY
			laserRot * 6
		}
	);
	laserMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });
	
	m4::Mat5 laserPointMat = MV;
	laserPointMat.translate(glm::vec4{ 0.0f, 0.73f, -2.2f - laserLength, 0.0001f });
	laserPointMat.scale(glm::vec4{ 0.1f,.1f,0.1,0.1f });
	laserPointMat.translate(glm::vec4{ -0.5f, -0.5f, -0.5f, -0.5f });
	laserPointMat *= m4::Rotor
	(
		{
			m4::wedge({1, 0, 0, 0}, {0, 1, 0, 0}), // XY
			laserRot*(- 12)
		}
	);
	

	const Shader* shader = ShaderManager::get("tetSolidColorNormalShader");

	shader->use();

	glUniform4f(glGetUniformLocation(shader->id(), "lightDir"), lightDir.x, lightDir.y, lightDir.z, lightDir.w);

	//LASER COLOR
	glUniform4f(glGetUniformLocation(shader->id(), "inColor"), .8, .8, .8, 1);

	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(laserMat) / sizeof(float), &laserMat[0][0]);
	if (player->keys.rightMouseDown && fuelLevel>0)
		renderer.render();

	//IRON COLOR
	glUniform4f(glGetUniformLocation(shader->id(), "inColor"), 166.0f / 255.0f, 164.0f / 255.0f, 158.0f / 255.0f, 1);


	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(handleMat) / sizeof(float), &handleMat[0][0]);
	renderer.render();
	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(slopeMat) / sizeof(float), &slopeMat[0][0]);
	renderer.render();
	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(lowerMat) / sizeof(float), &lowerMat[0][0]);
	renderer.render();
	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(upperMat) / sizeof(float), &upperMat[0][0]);
	renderer.render();
	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(buttMat) / sizeof(float), &buttMat[0][0]);
	renderer.render();
	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(upperLensHolderMat) / sizeof(float), &upperLensHolderMat[0][0]);
	renderer.render();
	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(lowerLensHolderMat) / sizeof(float), &lowerLensHolderMat[0][0]);
	renderer.render();

	// SOLENOID COLOR
	glUniform4f(glGetUniformLocation(shader->id(), "inColor"), 59.0f / 255.f, 180.0f / 255.f, 110.0f / 255.f, 1);

	if (!player->keys.rightMouseDown) {
		glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(button1Mat) / sizeof(float), &button1Mat[0][0]);
		renderer.render();
	}
	if (!isFlushing) {
		glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(button2Mat) / sizeof(float), &button2Mat[0][0]);
		renderer.render();
	}

	// HIGHLIGHTED SOLENOID COLOR

	glUniform4f(glGetUniformLocation(shader->id(), "inColor"), 59.0f / 255.f, 224.0f / 255.f, 110.0f / 255.f, 1);

	if (player->keys.rightMouseDown) {
		glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(button1Mat) / sizeof(float), &button1Mat[0][0]);
		renderer.render();
	}
	if (isFlushing) {
		glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(button2Mat) / sizeof(float), &button2Mat[0][0]);
		renderer.render();
	}
	// FUEL INDICATOR
	glUniform4f(glGetUniformLocation(shader->id(), "inColor"), fuelColor.r, fuelColor.g, fuelColor.b, 1);
	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(fuelMat) / sizeof(float), &fuelMat[0][0]);
	renderer.render();
	glUniform4f(glGetUniformLocation(shader->id(), "inColor"), fuelColor.r / 2, fuelColor.g / 2, fuelColor.b / 2, 1);
	glUniform1fv(glGetUniformLocation(shader->id(), "MV"), sizeof(fuelBackgroundMat) / sizeof(float), &fuelBackgroundMat[0][0]);
	renderer.render();
	
	// LENSES

	const Shader* slingshotShader = ShaderManager::get("projectileShader");

	slingshotShader->use();

	glUniform4f(glGetUniformLocation(slingshotShader->id(), "lightDir"), 0, 1, 0, 0);

	glUniform4f(glGetUniformLocation(slingshotShader->id(), "inColor"), 1, 0, 0, 0.7);
	glUniform1fv(glGetUniformLocation(slingshotShader->id(), "MV"), sizeof(lensRedMat) / sizeof(float), &lensRedMat[0][0]);
	ItemTool::rockRenderer->render();

	glUniform4f(glGetUniformLocation(slingshotShader->id(), "inColor"), 0, 1, 0, 0.7);
	glUniform1fv(glGetUniformLocation(slingshotShader->id(), "MV"), sizeof(lensGreenMat) / sizeof(float), &lensGreenMat[0][0]);
	ItemTool::rockRenderer->render();

	glUniform4f(glGetUniformLocation(slingshotShader->id(), "inColor"), 0, 0, 1, 0.7);
	glUniform1fv(glGetUniformLocation(slingshotShader->id(), "MV"), sizeof(lensBlueMat) / sizeof(float), &lensBlueMat[0][0]);
	ItemTool::rockRenderer->render();

	// LASER POINT
	glUniform4f(glGetUniformLocation(slingshotShader->id(), "inColor"), .8, .8, .8, 1);
	glUniform1fv(glGetUniformLocation(slingshotShader->id(), "MV"), sizeof(laserPointMat) / sizeof(float), &laserPointMat[0][0]);
	if (player->keys.rightMouseDown && fuelLevel > 0 && laserLength<effectiveDistance*7.5)
		ItemTool::rockRenderer->render();

	// COMPASS UI

	if (hasCompassEffect && inHand)
		CompassRenderer::renderHand(glm::mat4x4{ {0,0,0,1},{0,0,0,0},{0,0,1,0},{1,0,0,0} });
}

nlohmann::json ItemBeamCannon::saveAttributes() {
	return {{ "inventory", inventory.save()},{ "fuelLevel", fuelLevel }, { "isFuelDeadly", isFuelDeadly}, { "isSelectedFuelDeadly", isSelectedFuelDeadly} };
}

// Cloning item
std::unique_ptr<Item> ItemBeamCannon::clone() {
	auto result = std::make_unique<ItemBeamCannon>();

	result->inventory = inventory;
	result->fuelLevel = fuelLevel;
	result->isFuelDeadly = isFuelDeadly;
	result->isSelectedFuelDeadly = isSelectedFuelDeadly;

	result->reloadUpgrades();

	return result;
}

// Instantiating item
$hookStatic(std::unique_ptr<Item>, Item, instantiateItem, const stl::string& itemName, uint32_t count, const stl::string& type, const nlohmann::json& attributes) {
	if (type != "beamCannon") return original(itemName, count, type, attributes);

	auto result = std::make_unique<ItemBeamCannon>();
	
	result->inventory = InventoryGrid({ 2,3 });
	result->inventory.load(attributes["inventory"]);
	result->inventory.name = "beamCannonInventory";
	result->inventory.label = "Beam Cannon Upgrades:";

	result->fuelLevel = (float)attributes["fuelLevel"];
	result->isFuelDeadly = (bool)attributes["isFuelDeadly"];
	result->isSelectedFuelDeadly = (bool)attributes["isSelectedFuelDeadly"];

	result->inventory.renderPos= glm::ivec2{ 397,50 };
	result->count = 1;

	result->reloadUpgrades();

	return result;
}

// Glasses/Compass effect
$hook(bool, Player, isHoldingGlasses) {
	ItemBeamCannon* beamCannon;
	beamCannon = dynamic_cast<ItemBeamCannon*>(self->hotbar.getSlot(self->hotbar.selectedIndex)->get());
	if (!beamCannon) beamCannon = dynamic_cast<ItemBeamCannon*>(self->equipment.getSlot(0)->get());
	if (!beamCannon) return original(self);
	return original(self) || beamCannon->hasGlassesEffect;
}