#pragma once

#include "Texture2D.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Engine {
	class Material {
	public:
		Material(std::shared_ptr<Shader> shader)
			: _shader(shader) {}
		~Material() {}

		void Bind() const {
			for (auto& [bindingPoint, texture] : _textureMap) {
				texture->Bind(bindingPoint);
			}
			_shader->Bind();
		}

		void Unbind() const {
			_shader->Unbind();
		}

		void SetShader(std::shared_ptr<Shader> shader) { _shader = shader; }
		Shader& GetShader() { return *_shader; }

		void AddTexture(std::shared_ptr<Texture2D> texture, const std::string& uniformName, uint32_t index) { 
			_textureMap.emplace(index, texture);
			_shader->SetUniform(uniformName, (int)index);
		}
		Texture2D& GetTexture(uint32_t index) { return *_textureMap[index]; }
		uint32_t GetTextureCount() { return _textureMap.size(); }
	private:
		std::shared_ptr<Shader> _shader;
		std::unordered_map<uint32_t, std::shared_ptr<Texture2D>> _textureMap;
	};
}