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
	if (!HasTexture(name)) {
		ENGINE_ERROR("[Material] Texture '{0}' does not exist in the material.", name);
		return;
	}

	_textureMap[name].texture = texture;
}

bool Material::HasTexture(const std::string& name) const {
	return _textureMap.find(name) != _textureMap.end();
}

UniformTexture Material::GetTexture(const std::string& name) {
	if (!HasTexture(name)) {
		ENGINE_ERROR("[Material] Texture '{0}' does not exist in the material.", name);
		return { nullptr, 0 };
	}

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

std::optional<UniformValue> Material::GetUniformValue(const std::string& name) const {
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

std::shared_ptr<MaterialOverride> Material::CreateInstance() {
	return std::make_shared<MaterialOverride>(this);
}

// MaterialInstance implementation

MaterialOverride::MaterialOverride(Material* baseMaterial)
	: _baseMaterial(baseMaterial) {}

void MaterialOverride::Bind() const {
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

void MaterialOverride::Unbind() const {
	_baseMaterial->Unbind();
}

void MaterialOverride::SetShader(std::shared_ptr<Shader> shader) {
	_baseMaterial->SetShader(shader);
}

Shader& MaterialOverride::GetShader() {
	return _baseMaterial->GetShader();
}

const Shader& MaterialOverride::GetShader() const {
	return _baseMaterial->GetShader();
}

void MaterialOverride::SetTexture(const std::string& name, std::shared_ptr<BaseTexture> texture) {
	if (!_baseMaterial->HasTexture(name)) {
		ENGINE_ERROR("[MaterialInstance] Texture '{0}' does not exist in the base material.", name);
		return;
	}

	_overriddenTextures[name] = texture;
}

UniformTexture MaterialOverride::GetTexture(const std::string& name) {
	return { _overriddenTextures[name],_baseMaterial->GetTexture(name).bindingPoint };
}

uint32_t MaterialOverride::GetTextureCount() const {
	return (uint32_t)_overriddenTextures.size();
}

std::vector<std::string> Engine::MaterialOverride::GetTextureNames() const {
	std::vector<std::string> names;
	for (const auto& [name, _] : _overriddenTextures) {
		names.push_back(name);
	}
	return names;
}

void MaterialOverride::SetUniform(const std::string& name, UniformValue value) {
	if (!_baseMaterial->GetUniformValue(name).has_value()) {
		ENGINE_ERROR("Uniform '{0}' does not exist in the base material.", name);
		return;
	}
	_overriddenUniforms[name] = value;
}

void MaterialOverride::ResetUniform(const std::string& name) {
	_overriddenUniforms.erase(name);
}

std::optional<UniformValue> MaterialOverride::GetUniformValue(const std::string& name) const {
	auto it = _overriddenUniforms.find(name);
	if (it != _overriddenUniforms.end()) {
		return it->second;
	}
	return _baseMaterial->GetUniformValue(name);
}

std::vector<std::string> MaterialOverride::GetUniformNames() const {
	return _baseMaterial->GetUniformNames();
}

std::vector<std::string> MaterialOverride::GetOverriddenUniformNames() const {
	std::vector<std::string> names;
	for (const auto& [name, _] : _overriddenUniforms) {
		names.push_back(name);
	}
	return names;
}

bool MaterialOverride::IsUniformOverridden(const std::string& name) const {
	return _overriddenUniforms.find(name) != _overriddenUniforms.end();
}

std::vector<std::string> Engine::MaterialOverride::GetOverriddenTextureNames() const {
	std::vector<std::string> names;
	for (const auto& [name, _] : _overriddenTextures) {
		names.push_back(name);
	}
	return names;
}

bool MaterialOverride::IsTextureOverridden(const std::string& name) const {
	return _overriddenTextures.find(name) != _overriddenTextures.end();
}
