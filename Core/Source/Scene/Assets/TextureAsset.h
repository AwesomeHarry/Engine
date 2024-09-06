#pragma once

#include "Scene/Asset.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubemap.h"

namespace Engine {
	template<typename T>
	class BaseTextureAsset : public Asset<T> {
	public:
		BaseTextureAsset(AssetBank& assetBank, uint32_t id, AssetType type, const std::string& filepath)
			: Asset<T>(assetBank, id, "", type), _filepath(filepath) {}

		virtual nlohmann::json Serialize() const override {
			nlohmann::json json = Asset::Serialize();
			json["filepath"] = _filepath;
			return json;
		}

		virtual void Deserialize(const nlohmann::json& json) override {
			Asset::Deserialize(json);
			_filepath = json["filepath"];
		}
	protected:
		std::string _filepath;
	};

	class Texture2DAsset : public BaseTextureAsset<Texture2D> {
	public:
		Texture2DAsset(AssetBank& assetBank, uint32_t id, const std::string& filepath)
			: BaseTextureAsset<Texture2D>(assetBank, id, AssetType::Texture2D, filepath) {}
	protected:
		std::shared_ptr<Texture2D> CreateInstance() override {
			auto instance = Texture2D::Utils::FromFile(_filepath);
			if (!instance) {
				ENGINE_ERROR("[Texture2DAsset] Failed to load texture from file: {0}", _filepath);
				return nullptr;
			}
			return instance;
		}
	};

	class TextureCubemapAsset : public BaseTextureAsset<TextureCubemap> {
	public:
		TextureCubemapAsset(AssetBank& assetBank, uint32_t id, const std::string& filepath)
			: BaseTextureAsset<TextureCubemap>(assetBank, id, AssetType::Texture2D, filepath) {}
	protected:
		std::shared_ptr<TextureCubemap> CreateInstance() override {
			auto tex = Texture2D::Utils::FromFile(_filepath);
			auto instance = TextureCubemap::Utils::FromTexture2D(tex, TextureCubemap::Utils::Texture2DCubemapFormat::Equirectangle);
			if (!instance) {
				ENGINE_ERROR("[TextureCubemapAsset] Failed to load cubemap from file: {0}", _filepath);
				return nullptr;
			}
			return instance;
		}
	};
}
