#include "Material.h"

using namespace Engine;

// Material implementation

Material::Material(std::shared_ptr<Shader> shader)
	: _shader(shader) {}

void Material::Bind() const {
	for (const auto& [name, uniform] : _uniformMap) {
		if (uniform.isSampler) continue;
		std::visit([&, name = name](auto&& value) {
			_shader->SetUniform(name, value);
		}, uniform.value);
	}

	for (const auto& [bindingPoint, texture] : _textureMap) {
		texture->Bind(bindingPoint);
	}
}

void Material::Unbind() const {
	_shader->Unbind();
}

void Material::SetShader(std::shared_ptr<Shader> shader) {
	_shader = shader;
}

void Material::AddTexture(std::shared_ptr<BaseTexture> texture, const std::string& uniformName, uint32_t bindingPoint) {
	_textureMap[bindingPoint] = texture;
	_uniformMap[uniformName] = { (int)bindingPoint, true };
}

void Material::UpdateTexture(const std::string& uniformName, std::shared_ptr<BaseTexture> newTexture) {
	auto it = _uniformMap.find(uniformName);
	if (it != _uniformMap.end()) {
		uint32_t bindingPoint = std::get<int>(it->second.value);
		_textureMap[bindingPoint] = newTexture;
	}
}

BaseTexture& Material::GetTexture(const std::string& uniformName) {
	uint32_t bindingPoint = std::get<int>(_uniformMap[uniformName].value);
	return *_textureMap[bindingPoint];
}

uint32_t Material::GetTextureCount() const {
	return (uint32_t)_textureMap.size();
}

void Material::SetUniform(const std::string& name, bool value) {
	_uniformMap[name] = { value, false };
}

void Material::SetUniform(const std::string& name, int value) {
	_uniformMap[name] = { value, false };
}

void Material::SetUniform(const std::string& name, float value) {
	_uniformMap[name] = { value, false };
}

void Material::SetUniform(const std::string& name, const glm::vec2& value) {
	_uniformMap[name] = { value, false };
}

void Material::SetUniform(const std::string& name, const glm::vec3& value) {
	_uniformMap[name] = { value, false };
}

void Material::SetUniform(const std::string& name, const glm::vec4& value) {
	_uniformMap[name] = { value, false };
}

void Material::SetUniform(const std::string& name, const glm::mat2& value) {
	_uniformMap[name] = { value, false };
}

void Material::SetUniform(const std::string& name, const glm::mat3& value) {
	_uniformMap[name] = { value, false };
}

void Material::SetUniform(const std::string& name, const glm::mat4& value) {
	_uniformMap[name] = { value, false };
}

std::optional<UniformValue> Material::GetUniform(const std::string& name) const {
	auto it = _uniformMap.find(name);
	if (it != _uniformMap.end()) {
		return it->second.value;
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
	for (const auto& [name, uniform] : _overriddenUniforms) {
		if (uniform.isSampler) continue;
		std::visit([&, name = name](auto&& value) {
			_baseMaterial->GetShader().SetUniform(name, value);
		}, uniform.value);
	}

	for (const auto& [index, texture] : _overriddenTextures) {
		texture->Bind(index);
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

void MaterialInstance::AddTexture(std::shared_ptr<BaseTexture> texture, const std::string& uniformName, uint32_t bindingPoint) {
	_overriddenTextures[bindingPoint] = texture;
	_overriddenUniforms[uniformName] = { (int)bindingPoint, true };
}

void MaterialInstance::UpdateTexture(const std::string& uniformName, std::shared_ptr<BaseTexture> newTexture) {
	auto it = _overriddenUniforms.find(uniformName);
	if (it != _overriddenUniforms.end()) {
		uint32_t bindingPoint = std::get<int>(it->second.value);
		_overriddenTextures[bindingPoint] = newTexture;
	}
}

BaseTexture& MaterialInstance::GetTexture(const std::string& uniformName) {
	uint32_t bindingPoint = std::get<int>(_overriddenUniforms[uniformName].value);
	return *_overriddenTextures[bindingPoint];
}

uint32_t MaterialInstance::GetTextureCount() const {
	return (uint32_t)_overriddenTextures.size();
}

void MaterialInstance::SetUniform(const std::string& name, bool value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::SetUniform(const std::string& name, int value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::SetUniform(const std::string& name, float value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::SetUniform(const std::string& name, const glm::vec2& value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::SetUniform(const std::string& name, const glm::vec3& value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::SetUniform(const std::string& name, const glm::vec4& value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::SetUniform(const std::string& name, const glm::mat2& value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::SetUniform(const std::string& name, const glm::mat3& value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::SetUniform(const std::string& name, const glm::mat4& value) {
	if (!_baseMaterial->GetUniform(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = { value, false };
}

void MaterialInstance::ResetUniform(const std::string& name) {
	_overriddenUniforms.erase(name);
}

std::optional<UniformValue> MaterialInstance::GetUniform(const std::string& name) const {
	auto it = _overriddenUniforms.find(name);
	if (it != _overriddenUniforms.end()) {
		return it->second.value;
	}
	return _baseMaterial->GetUniform(name);
}

std::vector<std::string> MaterialInstance::GetUniformNames() const {
	return _baseMaterial->GetUniformNames();
}

bool MaterialInstance::IsUniformOverridden(const std::string& name) const {
	return _overriddenUniforms.find(name) != _overriddenUniforms.end();
}

std::vector<std::string> MaterialInstance::GetOverriddenUniformNames() const {
	std::vector<std::string> names;
	for (const auto& [name, _] : _overriddenUniforms) {
		names.push_back(name);
	}
	return names;
}
