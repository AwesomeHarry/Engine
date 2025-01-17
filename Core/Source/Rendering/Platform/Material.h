#pragma once

#include <string>
#include <memory>
#include <variant>
#include <unordered_map>
#include <optional>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture2D.h"

namespace Engine {
	using UniformValue = std::variant<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;
	struct UniformTexture { std::shared_ptr<BaseTexture> texture; uint32_t bindingPoint; };

	class IRenderableMaterial {
	public:
		virtual ~IRenderableMaterial() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	};

	class Material : public IRenderableMaterial {
	public:
		Material(std::shared_ptr<Shader> shader);
		~Material() override = default;

		void Bind() const override;
		void Unbind() const override;

		void SetShader(std::shared_ptr<Shader> shader);
		Shader& GetShader() { return *_shader; }
		const Shader& GetShader() const { return *_shader; }

		void SetTexture(const std::string& name, std::shared_ptr<BaseTexture> texture);
		bool HasTexture(const std::string& name) const;
		UniformTexture GetTexture(const std::string& name);
		uint32_t GetTextureCount() const;
		std::vector<std::string> GetTextureNames() const;

		void SetUniform(const std::string& name, UniformValue value);

		std::optional<UniformValue> GetUniformValue(const std::string& name) const;
		std::vector<std::string> GetUniformNames() const;

	private:
		void SetDefaultUniformsAndTextures(Shader& shader);

	protected:
		std::shared_ptr<Shader> _shader;
		std::unordered_map<std::string, UniformValue> _uniformMap;
		std::unordered_map<std::string, UniformTexture> _textureMap;
	};
}
