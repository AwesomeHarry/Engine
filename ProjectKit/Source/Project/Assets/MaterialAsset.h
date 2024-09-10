#pragma once

#include "Project/AssetSystem.h"
#include "Rendering/Platform/Material.h"

#include "ShaderAsset.h"
#include "TextureAsset.h"

#include "Serializers/UniformSerializer.h"

namespace Engine {
	class MaterialAsset : public Asset {
	public:
		MaterialAsset(AssetBank& assetBank) : Asset(assetBank, AssetType::Material) {}

		void Load() override {
			// Resolve Shader
			auto shaderAsset = _shaderRef.Resolve<ShaderAsset>(_assetBank);
			auto shader = shaderAsset->GetInternal();

			// Create Internal Material
			_internalMaterial = std::make_shared<Material>(shader);

			// Set Uniforms
			for (auto& [name, value] : _uniforms) {
				_internalMaterial->SetUniform(name, value);
			}

			// Set Textures
			for (auto& [name, textureRef] : _textures) {
				auto textureAsset = textureRef.Resolve<TextureAsset>(_assetBank);
				auto texture = textureAsset->GetInternal();
				_internalMaterial->SetTexture(name, texture);
			}

			_loaded = true;
		}

		void Unload() override {
			_internalMaterial = nullptr;
			_loaded = false;
		}

		void SetShader(AssetRef shaderRef) {
			_shaderRef = shaderRef;
			if (_loaded) {
				auto shaderAsset = _shaderRef.Resolve<ShaderAsset>(_assetBank);
				auto shader = shaderAsset->GetInternal();
				_internalMaterial->SetShader(shader);
			}
		}

		void SetUniform(const std::string& name, const UniformValue& value) {
			_uniforms[name] = value;
			if (_loaded) {
				_internalMaterial->SetUniform(name, value);
			}
		}

		void SetTexture(const std::string& name, AssetRef textureRef) {
			_textures[name] = textureRef;
			if (_loaded) {
				auto textureAsset = textureRef.Resolve<TextureAsset>(_assetBank);
				auto texture = textureAsset->GetInternal();
				_internalMaterial->SetTexture(name, texture);
			}
		}

		nlohmann::json Serialize() const override {
			nlohmann::json data = Asset::Serialize();
			data["shaderRef"] = _shaderRef;
			data["uniforms"] = _uniforms;
			data["textures"] = _textures;
			return data;
		}

		void Deserialize(const nlohmann::json& data) override {
			Asset::Deserialize(data);
			_shaderRef = data["shaderRef"].get<AssetRef>();
			_uniforms = data["uniforms"].get<std::unordered_map<std::string, UniformValue>>();
			_textures = data["textures"].get<std::unordered_map<std::string, AssetRef>>();
			if (_loaded) Unload();
		}

		std::shared_ptr<Material> GetInternal() {
			if (!_loaded) Load();
			return _internalMaterial;
		}

	private:
		std::shared_ptr<Material> _internalMaterial = nullptr;

		AssetRef _shaderRef;
		std::unordered_map<std::string, UniformValue> _uniforms;
		std::unordered_map<std::string, AssetRef> _textures;
	};
}