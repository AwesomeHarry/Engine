#pragma once

#include "Asset.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubemap.h"

namespace Engine {
	class Texture2DAsset : public Asset<Texture2D> {
	public:
		Texture2DAsset() { type = AssetType::Texture2D; }

		nlohmann::json Serialize() const override {
			nlohmann::json j;
			to_json(j, static_cast<const AssetInfo&>(*this));
			j["filepath"] = _filepath;
			return j;
		}

		void Deserialize(const nlohmann::json& j, AssetManager& assetManager) override {
			from_json(j, static_cast<AssetInfo&>(*this));
			j.at("filepath").get_to(_filepath);
		}
	protected:
		std::shared_ptr<Texture2D> Create(AssetManager& assetManager) override {
			auto instance = Texture2D::Utils::FromFile(_filepath);
			if (!instance) {
				ENGINE_ERROR("[Texture2DAsset] Failed to load texture from file: {0}", _filepath);
				return nullptr;
			}
			return instance;
		}
	private:
		std::string _filepath;
	};

	class TextureCubemapAsset : public Asset<TextureCubemap> {
	public:
		TextureCubemapAsset() { type = AssetType::TextureCubemap; }

		nlohmann::json Serialize() const override {
			nlohmann::json j;
			to_json(j, static_cast<const AssetInfo&>(*this));
			j["filepath"] = _filepath;
			return j;
		}

		void Deserialize(const nlohmann::json& j, AssetManager& assetManager) override {
			from_json(j, static_cast<AssetInfo&>(*this));
			j.at("filepath").get_to(_filepath);
		}
	protected:
		std::shared_ptr<TextureCubemap> Create(AssetManager& assetManager) override {
			auto texture = Texture2D::Utils::FromFile(_filepath);
			if (!texture) {
				ENGINE_ERROR("[TextureCubemapAsset] Failed to load texture from file: {0}", _filepath);
				return nullptr;
			}
			auto cubemap = TextureCubemap::Utils::FromTexture2D(texture, TextureCubemap::Utils::Texture2DCubemapFormat::Equirectangle);
			if (!cubemap) {
				ENGINE_ERROR("[TextureCubemapAsset] Failed to create cubemap from texture: {0}", _filepath);
				return nullptr;
			}
			return cubemap;
		}
	private:
		std::string _filepath;
	};
}
