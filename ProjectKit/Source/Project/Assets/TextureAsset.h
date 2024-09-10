#pragma once

#include "Project/AssetSystem.h";
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubeMap.h"

namespace Engine {
	class TextureAsset : public Asset {
	public:
		TextureAsset(AssetBank& assetBank) : Asset(assetBank, AssetType::Texture), _type(TextureType::Tex2D) {}

		void Load() override {
			// Load texture from file
			switch (_type) {
			case TextureType::Tex2D:
				_internalTexture = Texture2D::Utils::FromFile(_texturePath);
				break;
			case TextureType::TexCubemap:
				auto texture2D = Texture2D::Utils::FromFile(_texturePath);
				_internalTexture = TextureCubemap::Utils::FromTexture2D(texture2D, TextureCubemap::Utils::Texture2DCubemapFormat::Equirectangle);
				break;
			}
			_loaded = true;
		}

		void Unload() override {
			_internalTexture = nullptr;
			_loaded = false;
		}

		nlohmann::json Serialize() const override {
			nlohmann::json data = Asset::Serialize();
			data["texturePath"] = _texturePath;
			data["type"] = _type;
			return data;
		}

		void Deserialize(const nlohmann::json& data) override {
			Asset::Deserialize(data);
			_texturePath = data["texturePath"];
			_type = data["type"];
			if (_loaded) Unload();
		}

		std::shared_ptr<BaseTexture> GetInternal() {
			if (!_loaded) Load();
			return _internalTexture;
		}

		const std::string& GetTexturePath() const { return _texturePath; }
		void SetTexturePath(const std::string& texturePath) {
			if (texturePath != _texturePath) // If path changed, unload the texture
				Unload();
			_texturePath = texturePath;
		}

		TextureType GetTextureType() const { return _type; }
		void SetType(TextureType type) {
			if (type != _type) // If type changed, unload the texture
				Unload();
			_type = type;
		}

	protected:
		std::shared_ptr<BaseTexture> _internalTexture = nullptr;

		std::string _texturePath;
		TextureType _type;
	};
}
