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

	class IMaterial {
	public:
		virtual ~IMaterial() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetShader(std::shared_ptr<Shader> shader) = 0;
		virtual Shader& GetShader() = 0;
		virtual const Shader& GetShader() const = 0;

		virtual void SetTexture(const std::string& uniformName, std::shared_ptr<BaseTexture> texture) = 0;
		virtual UniformTexture GetTexture(const std::string& name) = 0;
		virtual uint32_t GetTextureCount() const = 0;
		virtual std::vector<std::string> GetTextureNames() const = 0;

		virtual void SetUniform(const std::string& name, UniformValue value) = 0;

		virtual std::optional<UniformValue> GetUniformValue(const std::string& name) const = 0;
		virtual std::vector<std::string> GetUniformNames() const = 0;
	};

	class Material : public IMaterial {
	public:
		Material(std::shared_ptr<Shader> shader);
		~Material() override = default;

		void Bind() const override;
		void Unbind() const override;

		void SetShader(std::shared_ptr<Shader> shader) override;
		Shader& GetShader() override { return *_shader; }
		const Shader& GetShader() const override { return *_shader; }

		void SetTexture(const std::string& name, std::shared_ptr<BaseTexture> texture) override;
		bool HasTexture(const std::string& name) const;
		UniformTexture GetTexture(const std::string& name) override;
		uint32_t GetTextureCount() const override;
		std::vector<std::string> GetTextureNames() const override;

		void SetUniform(const std::string& name, UniformValue value) override;

		std::optional<UniformValue> GetUniformValue(const std::string& name) const override;
		std::vector<std::string> GetUniformNames() const override;

	private:
		void SetDefaultUniformsAndTextures(Shader& shader);

	protected:
		std::shared_ptr<Shader> _shader;
		std::unordered_map<std::string, UniformValue> _uniformMap;
		std::unordered_map<std::string, UniformTexture> _textureMap;
	};
}
