#pragma once
#include <json.hpp>
#include <string>

namespace Engine {
	enum class AssetType { Unknown = 0, Shader, Texture2D, TextureCubemap, Material, Mesh, Scene };

	NLOHMANN_JSON_SERIALIZE_ENUM(AssetType, {
	{AssetType::Unknown, "Unknown"},
	{AssetType::Shader, "Shader"},
	{AssetType::Texture2D, "Texture2D"},
	{AssetType::TextureCubemap, "TextureCubemap"},
	{AssetType::Material, "Material"},
	{AssetType::Mesh, "Mesh"},
	{AssetType::Scene, "Scene"}
								 });

	inline std::string AssetTypeToString(AssetType type) {
		switch (type) {
		case AssetType::Unknown:		return "Unknown";
		case AssetType::Shader:			return "Shader";
		case AssetType::Texture2D:		return "Texture2D";
		case AssetType::TextureCubemap: return "TextureCubemap";
		case AssetType::Material:		return "Material";
		case AssetType::Mesh:			return "Mesh";
		case AssetType::Scene:			return "Scene";
		default: return "[AssetTypeToString] Unimplemented string converter.";
		}
	}
}