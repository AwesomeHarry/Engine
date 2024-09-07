#include "AssetBank.h"
#include "Asset.h"

using namespace Engine;

bool AssetBank::HasAsset(uint32_t id) {
	return _assets.find(id) != _assets.end();
}

void AssetBank::DeleteAsset(uint32_t id) {
	auto it = _assets.find(id);
	if (it != _assets.end()) {
		_assets.erase(it);
	}
}

void AssetBank::DeleteAll() {
	_assets.clear();
}

uint32_t AssetBank::GenerateID() {
	return _idCounter++;
}
