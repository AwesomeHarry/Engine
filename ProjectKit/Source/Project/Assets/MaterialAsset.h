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
			if (!_shaderRef.IsValid()) {
				ENGINE_ERROR("[MaterialAsset::Load] Shader Reference is Invalid");
				return;
			}
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
				if (!textureRef.IsValid()) continue;
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

		// TODO: Add Default Uniforms / Textures + Name Checking
		void SetShader(AssetRef shaderRef) {
			_shaderRef = shaderRef;
			auto shaderAsset = _shaderRef.Resolve<ShaderAsset>(_assetBank);
			bool shaderLoaded = shaderAsset->IsLoaded();
			
			auto shader = shaderAsset->GetInternal();
			{ // Set Default Uniforms using a Temp Material
				Material tempMaterial(shader);
				auto uniformNames = tempMaterial.GetUniformNames();
				for (auto& uniformName : uniformNames) {
					auto uniformValue = tempMaterial.GetUniformValue(uniformName).value();
					_uniforms[uniformName] = uniformValue;
				}
				auto textureNames = tempMaterial.GetTextureNames();
				for (auto& textureName : textureNames) {
					_textures[textureName] = AssetRef::Invalid();
				}
			}

			if (_loaded) {
				_internalMaterial->SetShader(shader);
			}

			if (!shaderLoaded) shaderAsset->Unload();
		}

		void SetUniform(const std::string& name, const UniformValue& value) {
			if (!UniformExists(name)) {
				ENGINE_ERROR("[MaterialAsset::SetUniform] Uniform '{0}' does not exist", name);
				return;
			}

			_uniforms[name] = value;

			if (_loaded) {
				_internalMaterial->SetUniform(name, value);
			}
		}

		void SetTexture(const std::string& name, AssetRef textureRef) {
			if (!TextureExists(name)) {
				ENGINE_ERROR("[MaterialAsset::SetTexture] Texture '{0}' does not exist", name);
				return;
			}

			_textures[name] = textureRef;

			if (_loaded) {
				auto textureAsset = textureRef.Resolve<TextureAsset>(_assetBank);
				auto texture = textureAsset->GetInternal();
				_internalMaterial->SetTexture(name, texture);
			}
		}

		bool UniformExists(const std::string& name) const {
			return _uniforms.find(name) != _uniforms.end();
		}

		bool TextureExists(const std::string& name) const {
			return _textures.find(name) != _textures.end();
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

		AssetRef GetShaderRef() const { return _shaderRef; }

	private:
		std::shared_ptr<Material> _internalMaterial = nullptr;

		AssetRef _shaderRef;
		std::unordered_map<std::string, UniformValue> _uniforms;
		std::unordered_map<std::string, AssetRef> _textures;
	};
}