#pragma once

#include "hypercore.h"

using namespace fdm;

namespace hypercore {
    class SoundController {
    public:
        inline static std::vector<std::string> sounds{};

        inline static void addSound(std::string& soundPath){
            soundPath = std::format("../../{}/{}", fdm::getModPath(fdm::modID), soundPath);
            sounds.push_back(soundPath);
        }

        inline static void loadSounds() {
            for (auto& sound : sounds)
                if (!AudioManager::loadSound(sound)) Console::printLine("Cannot load sound: ", sound);
                else Console::printLine("Successfully loaded sound: ", sound);
        }
    };
}