#pragma once

#include "Asset.h"
#include "Rendering/Platform/Material.h"

namespace Engine {
	class MaterialAssetInstance : public Material {
	public:
		MaterialAssetInstance(std::shared_ptr<Shader> shader, MaterialAsset* owner)
			: Material(shader), _owner(owner) {}

		void SetUniform(const std::string& name, UniformValue value) override {
			Material::SetUniform(name, value);
			_owner->UpdateUniform(name, value);
		}

		void SetTexture(const std::string& name, const AssetRef& texture) {
			Material::SetTexture(name, texture.Resolve<BaseTexture>());
			_owner->UpdateTexture(name, texture);
		}
	protected:
		using Material::SetTexture;
	private:
		MaterialAsset* _owner;
	};

	class MaterialAsset : public Asset<MaterialAssetInstance> {
	public:
		MaterialAsset() { type = AssetType::Material; }

		void UpdateUniform(const std::string& name, UniformValue value) {
			_uniformValues[name] = value;
		}

		void UpdateTexture(const std::string& name, const AssetRef& texture) {
			_textures[name] = texture;
		}

		nlohmann::json Serialize() const override {
			nlohmann::json j;
			to_json(j, static_cast<const AssetInfo&>(*this));
			j["shader"] = _shaderRef;



			j["textures"] = _textures;
			return j;
		}
	protected:
		std::shared_ptr<Material> Create(AssetManager& assetManager) override;
	private:
		AssetRef _shaderRef;
		std::unordered_map<std::string, UniformValue> _uniformValues;
		std::unordered_map<std::string, std::pair<AssetRef, uint32_t>> _textures;
	};

	inline void to_json(nlohmann::json& j, const UniformValue& uv) {
		j = nlohmann::json{
			{"type", uv.},
			{"value", uv.value}
		};
	}
}