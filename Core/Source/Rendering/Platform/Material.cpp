#include "Material.h"

using namespace Engine;

// Material implementation

Material::Material(std::shared_ptr<Shader> shader)
	: _shader(shader) {
	auto& uniforms = shader->GetUniformInfos();
	uint32_t textureIndex = 0;
	for (const auto& [name, info] : uniforms) {
		if (info.type == UniformType::Sampler2D || info.type == UniformType::SamplerCube) {
			_textureMap[info.name] = { nullptr, textureIndex };
			shader->SetUniform(info.name, (int)textureIndex);
			textureIndex++;
		}
		else {
			switch (info.type) {
			case UniformType::Bool:
				_uniformMap[name] = false;
				break;
			case UniformType::Int:
				_uniformMap[name] = 0;
				break;
			case UniformType::Float:
				_uniformMap[name] = 0.0f;
				break;
			case UniformType::Vec2:
				_uniformMap[name] = glm::vec2(0.0f);
				break;
			case UniformType::Vec3:
				_uniformMap[name] = glm::vec3(0.0f);
				break;
			case UniformType::Vec4:
				_uniformMap[name] = glm::vec4(0.0f);
				break;
			case UniformType::Mat2:
				_uniformMap[name] = glm::mat2(1.0f);
				break;
			case UniformType::Mat3:
				_uniformMap[name] = glm::mat3(1.0f);
				break;
			case UniformType::Mat4:
				_uniformMap[name] = glm::mat4(1.0f);
				break;
			}
		}
	}
}

void Material::Bind() const {
	for (const auto& [name, value] : _uniformMap) {
		std::visit([&, name = name](auto&& value) {
			_shader->SetUniform(name, value);
		}, value);
	}

	for (const auto& [name, uniformTexture] : _textureMap) {
		if (uniformTexture.texture == nullptr) continue;
		uniformTexture.texture->Bind(uniformTexture.bindingPoint);
	}
}

void Material::Unbind() const {
	_shader->Unbind();
}

void Material::SetShader(std::shared_ptr<Shader> shader) {
	_shader = shader;
}

void Material::SetTexture(const std::string& name, std::shared_ptr<BaseTexture> texture) {
	_textureMap[name].texture = texture;
}

UniformTexture Material::GetTexture(const std::string& name) {
	return _textureMap[name];
}

uint32_t Material::GetTextureCount() const {
	return (uint32_t)_textureMap.size();
}

std::vector<std::string> Engine::Material::GetTextureNames() const {
	std::vector<std::string> names;
	for (const auto& [name, _] : _textureMap) {
		names.push_back(name);
	}
	return names;
}

void Material::SetUniform(const std::string& name, UniformValue value) {
	_uniformMap[name] = value;
}

std::optional<UniformValue> Material::GetUniform(const std::string& name) const {
	auto it = _uniformMap.find(name);
	if (it != _uniformMap.end()) {
		return it->second;
	}
	return std::nullopt;
}

std::vector<std::string> Material::GetUniformNames() const {
	std::vector<std::string> names;
	for (const auto& [name, _] : _uniformMap) {
		names.push_back(name);
	}
	return names;
}

std::shared_ptr<MaterialInstance> Material::CreateInstance() {
	return std::make_shared<MaterialInstance>(shared_from_this());
}

// MaterialInstance implementation

MaterialInstance::MaterialInstance(std::shared_ptr<Material> baseMaterial)
	: _baseMaterial(baseMaterial) {}

void MaterialInstance::Bind() const {
	_baseMaterial->Bind();

	for (const auto& [name, value] : _overriddenUniforms) {
		std::visit([&, name = name](auto&& value) {
			_baseMaterial->GetShader().SetUniform(name, value);
		}, value);
	}

	for (const auto& [name, uniformTexture] : _overriddenTextures) {
		uniformTexture->Bind(_baseMaterial->GetTexture(name).bindingPoint);
	}
}

void MaterialInstance::Unbind() const {
	_baseMaterial->Unbind();
}

void MaterialInstance::SetShader(std::shared_ptr<Shader> shader) {
	_baseMaterial->SetShader(shader);
}

Shader& MaterialInstance::GetShader() {
	return _baseMaterial->GetShader();
}

const Shader& MaterialInstance::GetShader() const {
	return _baseMaterial->GetShader();
}

void MaterialInstance::SetTexture(const std::string& name, std::shared_ptr<BaseTexture> texture) {
	_overriddenTextures[name] = texture;
}

UniformTexture MaterialInstance::GetTexture(const std::string& name) {
	return { _overriddenTextures[name],_baseMaterial->GetTexture(name).bindingPoint };
}

uint32_t MaterialInstance::GetTextureCount() const {
	return (uint32_t)_overriddenTextures.size();
}

std::vector<std::string> Engine::MaterialInstance::GetTextureNames() const {
	std::vector<std::string> names;
	for (const auto& [name, _] : _overriddenTextures) {
		names.push_back(name);
	}
	return names;
}

void MaterialInstance::SetUniform(const std::string& name, UniformValue value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = value;
}

void MaterialInstance::ResetUniform(const std::string& name) {
	_overriddenUniforms.erase(name);
}

std::optional<UniformValue> MaterialInstance::GetUniform(const std::string& name) const {
	auto it = _overriddenUniforms.find(name);
	if (it != _overriddenUniforms.end()) {
		return it->second;
	}
	return _baseMaterial->GetUniform(name);
}

std::vector<std::string> MaterialInstance::GetUniformNames() const {
	return _baseMaterial->GetUniformNames();
}

bool MaterialInstance::IsUniformOverridden(const std::string& name) const {
	return _overriddenUniforms.find(name) != _overriddenUniforms.end();
}

bool MaterialInstance::IsTextureOverridden(const std::string& name) const {
	return _overriddenTextures.find(name) != _overriddenTextures.end();
}

std::vector<std::string> MaterialInstance::GetOverriddenUniformNames() const {
	std::vector<std::string> names;
	for (const auto& [name, _] : _overriddenUniforms) {
		names.push_back(name);
	}
	return names;
}
