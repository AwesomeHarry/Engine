#pragma once

#include "Project/AssetSystem.h";
#include "Rendering/Platform/Shader.h"

namespace Engine {
	class ShaderAsset : public Asset {
	public:
		ShaderAsset(AssetBank& assetBank) : Asset(assetBank, AssetType::Shader) {}

		void Load() override {
			// Load shader from file
			_internalShader = Shader::Utils::FromFile(_shaderPath);
			for (auto& [blockName, bindingPoint] : _uniformBlockMap) {
				_internalShader->BindUniformBlock(blockName, bindingPoint);
			}
			_loaded = true;
		}

		void Unload() override {
			_internalShader = nullptr;
			_loaded = false;
		}

		void SetUniformBlock(const std::string& blockName, uint32_t bindingPoint) {
			_uniformBlockMap[blockName] = bindingPoint;
			if (_loaded) {
				_internalShader->BindUniformBlock(blockName, bindingPoint);
			}
		}

		void SetShaderPath(const std::string& shaderPath) { 
			if (shaderPath != _shaderPath) // If path changed, unload the shader
				Unload();
			_shaderPath = shaderPath; 
		}
		const std::string& GetShaderPath() const { return _shaderPath; }

		std::shared_ptr<Shader> GetInternal() {
			if (!_loaded) Load();
			return _internalShader;
		}

		nlohmann::json Serialize() const override {
			nlohmann::json data = Asset::Serialize();
			data["shaderPath"] = _shaderPath;
			data["uniformBlockMap"] = _uniformBlockMap;
			return data;
		}

		void Deserialize(const nlohmann::json& data) override {
			Asset::Deserialize(data);
			_shaderPath = data["shaderPath"];
			_uniformBlockMap = data["uniformBlockMap"];
			if (_loaded) Unload();
		}

	private:
		std::shared_ptr<Shader> _internalShader = nullptr;
		std::string _shaderPath;
		std::map<std::string, uint32_t> _uniformBlockMap;
	};
}
