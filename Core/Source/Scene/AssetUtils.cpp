#include "AssetUtils.h"

#include "Assets/ShaderAsset.h"
#include "Assets/TextureAsset.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"

std::shared_ptr<IAsset> Engine::AssetUtils::LoadAssetFromJson(AssetBank& assetBank, const nlohmann::json& assetJson) {
	AssetType type = assetJson["type"].get<AssetType>();
	std::string name = assetJson["name"].get<std::string>();
	uint32_t id = assetJson["id"].get<uint32_t>();

	std::shared_ptr<IAsset> asset = nullptr;

	switch (type) {
	case AssetType::Shader:
	{
		std::string filepath = assetJson["filepath"].get<std::string>();
		asset = std::make_shared<ShaderAsset>(assetBank, id, filepath);
		break;
	}
	case AssetType::Texture2D:
	{
		std::string filepath = assetJson["filepath"].get<std::string>();
		asset = std::make_shared<Texture2DAsset>(assetBank, id, filepath);
		break;
	}
	case AssetType::TextureCubemap:
	{
		std::string filepath = assetJson["filepath"].get<std::string>();
		asset = std::make_shared<TextureCubemapAsset>(assetBank, id, filepath);
		break;
	}
	case AssetType::Material:
	{
		uint32_t shaderId = assetJson["shader"].get<uint32_t>();
		asset = std::make_shared<MaterialAsset>(assetBank, id, shaderId);
		break;
	}
	case AssetType::Mesh:
	{
		std::string filepath = assetJson["filepath"].get<std::string>();
		uint32_t meshIndex = assetJson["meshIndex"].get<uint32_t>();
		asset = std::make_shared<MeshAsset>(assetBank, id, filepath, meshIndex);
		break;
	}
	default:
		ENGINE_ERROR("[AssetUtils::LoadAsset] Unimplemented load for asset type: {0}", AssetTypeToString(type));
		return nullptr;
	}

	assetBank.RegisterAsset(asset);
	asset->Deserialize(assetJson);

	ENGINE_INFO("[AssetUtils::LoadAsset] Loaded asset '{0}' with id {1}", name, id);

	return asset;
};