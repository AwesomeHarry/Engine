#include "AssetManager.h"

using namespace Engine;

std::filesystem::path AssetManager::_projectRoot = std::filesystem::current_path();
entt::registry AssetManager::_registry;
uint32_t AssetManager::_idCounter = 0;
