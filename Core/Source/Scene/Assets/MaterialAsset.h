#pragma once

#include "Scene/Asset.h"
#include "Scene/AssetRef.h"
#include "Rendering/Platform/Material.h"
#include "Util/Serializers/UniformSerializer.h"

#include "ShaderAsset.h"
#include "TextureAsset.h"

namespace Engine {
	class MaterialAssetInstanceOverride;

	class MaterialAssetInstance : public Material {
	public:
		MaterialAssetInstance(AssetBank& assetBank, AssetRef shaderRef);

		void SetShaderRef(const AssetRef& shaderRef);
		const AssetRef& GetShaderRef() const { return _shaderRef; }

		void SetTextureRef(const std::string& name, const AssetRef& textureRef);
		const AssetRef& GetTextureRef(const std::string& name) const;

		std::shared_ptr<MaterialOverride> CreateInstance() override;

	private:
		AssetBank& _assetBank;

		AssetRef _shaderRef;
		std::unordered_map<std::string, AssetRef> _textureRefs;
	};

	class MaterialAssetInstanceOverride : public MaterialOverride {
	public:
		MaterialAssetInstanceOverride(AssetBank& assetBank, MaterialAssetInstance* materialAssetInstance);

		void SetShaderRef(const AssetRef& shaderRef);
		const AssetRef& GetShaderRef() const { return _shaderRef; }

		void SetTextureRef(const std::string& name, const AssetRef& textureRef);
		const AssetRef& GetTextureRef(const std::string& name) const;

	private:
		AssetBank& _assetBank;
		MaterialAssetInstance* _baseMaterialAssetInstance;

		AssetRef _shaderRef;
		std::unordered_map<std::string, AssetRef> _overridenTextureRefs;
	};

	class MaterialAsset : public Asset<MaterialAssetInstance> {
	public:
		MaterialAsset(AssetBank& assetBank, uint32_t id, uint32_t shaderId)
			: Asset(assetBank, id, "", AssetType::Material), _shaderId(shaderId) {}

		virtual nlohmann::json Serialize() const override {
			nlohmann::json json = Asset::Serialize();

			if (!IsInstantiated()) {
				json["shader"] = _shaderId;
				json["uniforms"] = _uniformValues;
				json["textures"] = _textureIds;
			}
			else {
				json["shader"] = _instance->GetShaderRef();

				// Serialize uniforms
				json["uniforms"] = nlohmann::json::object();
				auto uniformNames = _instance->GetUniformNames();
				for (const auto& uniformName : uniformNames) {
					json["uniforms"][uniformName] = _instance->GetUniformValue(uniformName).value();
				};

				// Serialize textures
				json["textures"] = nlohmann::json::object();
				auto textureNames = _instance->GetTextureNames();
				for (const auto& textureName : textureNames) {
					json["textures"][textureName] = _instance->GetTextureRef(textureName);
				}
			}

			return json;
		}

		virtual void Deserialize(const nlohmann::json& j) override {
			Asset::Deserialize(j);

			_shaderId = j["shader"].get<uint32_t>();

			// Deserialize uniforms
			auto uniforms = j["uniforms"];
			if (!uniforms.is_object())
				ENGINE_ERROR("[MaterialAsset::Deserialize] Uniforms is not an object");

			_uniformValues = uniforms.get<std::map<std::string, UniformValue>>();

			// Deserialize textures
			auto textures = j["textures"];
			if (!textures.is_object())
				ENGINE_ERROR("[MaterialAsset::Deserialize] Textures is not an object");

			_textureIds = textures.get<std::unordered_map<std::string, uint32_t>>();
		}

	protected:
		virtual std::shared_ptr<MaterialAssetInstance> CreateInstance() {
			auto instance = std::make_shared<MaterialAssetInstance>(_assetBank, AssetRef(_shaderId));

			for (const auto& [name, textureId] : _textureIds)
				instance->SetTextureRef(name, AssetRef(textureId));

			for (const auto& [name, value] : _uniformValues)
				instance->SetUniform(name, value);

			return instance;
		}
	private:
		uint32_t _shaderId;
		std::unordered_map<std::string, uint32_t> _textureIds;
		std::map<std::string, UniformValue> _uniformValues;
	};
}
