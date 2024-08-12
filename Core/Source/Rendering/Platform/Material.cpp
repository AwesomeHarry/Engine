#include "Material.h"
#include "Logging/Logging.h"

#include <glad/glad.h>

using namespace Engine;

Material::Material(std::shared_ptr<Shader> shader)
	: _shader(shader) {
	auto& uniformInfos = _shader->GetUniformInfos();
	for (const auto& [name, info] : uniformInfos) {
		switch (info.type) {
		case UniformType::Bool: SetUniform(name, false); break;
		case UniformType::Int: SetUniform(name, 0); break;
		case UniformType::Float: SetUniform(name, 0.0f); break;
		case UniformType::Vec2: SetUniform(name, glm::vec2(0.0f)); break;
		case UniformType::Vec3: SetUniform(name, glm::vec3(0.0f)); break;
		case UniformType::Vec4: SetUniform(name, glm::vec4(0.0f)); break;
		case UniformType::Mat2: SetUniform(name, glm::mat2(1.0f)); break;
		case UniformType::Mat3: SetUniform(name, glm::mat3(1.0f)); break;
		case UniformType::Mat4: SetUniform(name, glm::mat4(1.0f)); break;
		}
	}
}

Material::~Material() {}

void Material::Bind() const {
	// Bind Textures
	for (auto& [bindingPoint, texture] : _textureMap)
		texture->Bind(bindingPoint);

	// Set Uniforms
	for (const auto& [name, uniform] : _uniformMap) {
		std::visit([this, name = name](auto&& arg) {
			_shader->SetUniform(name, arg);
				   }, uniform.value);
	}

	// Bind Shader for further use
	_shader->Bind();
}

void Material::Unbind() const {
	_shader->Unbind();
}

void Material::SetShader(std::shared_ptr<Shader> shader) {
	_shader = shader;
}

Shader& Engine::Material::GetShader() {
	return *_shader;
}

const Shader& Material::GetShader() const {
	return *_shader;
}

void Material::AddTexture(std::shared_ptr<Texture2D> texture, const std::string& uniformName, uint32_t index) {
	_textureMap.emplace(index, texture);
	SetUniform(uniformName, (int)index);
	auto it = _uniformMap.find(uniformName);
	it->second.isSampler = true;
}

void Material::UpdateTexture(const std::string& uniformName, std::shared_ptr<Texture2D> newTexture) {
	for (auto& [index, texture] : _textureMap) {
		if (GetUniform<int>(uniformName) == (int)index) {
			texture = newTexture;
			return;
		}
	}

	// If texture not found, add it as a new texture
	AddTexture(newTexture, uniformName, GetTextureCount());
}

Texture2D& Material::GetTexture(uint32_t index) {
	return *_textureMap[index];
}

uint32_t Material::GetTextureCount() {
	return _textureMap.size();
}

void Material::SetUniform(const std::string& name, bool value) { _uniformMap[name].value = value; }
void Material::SetUniform(const std::string& name, int value) { _uniformMap[name].value = value; }
void Material::SetUniform(const std::string& name, float value) { _uniformMap[name].value = value; }
void Material::SetUniform(const std::string& name, const glm::vec2& value) { _uniformMap[name].value = value; }
void Material::SetUniform(const std::string& name, const glm::vec3& value) { _uniformMap[name].value = value; }
void Material::SetUniform(const std::string& name, const glm::vec4& value) { _uniformMap[name].value = value; }
void Material::SetUniform(const std::string& name, const glm::mat2& value) { _uniformMap[name].value = value; }
void Material::SetUniform(const std::string& name, const glm::mat3& value) { _uniformMap[name].value = value; }
void Material::SetUniform(const std::string& name, const glm::mat4& value) { _uniformMap[name].value = value; }

template<typename T>
T Material::GetUniform(const std::string& name) const {
	auto it = _uniformMap.find(name);
	if (it != _uniformMap.end()) {
		return std::get<T>(it->second.value);
	}
	ENGINE_ERROR("Uniform not found: {}", name);
	return T(); // Return default-constructed value
}

std::shared_ptr<Material> Material::Copy() {
	auto newMaterial = std::make_shared<Material>(_shader);
	newMaterial->_textureMap = _textureMap;
	newMaterial->_uniformMap = _uniformMap;
	return newMaterial;
}