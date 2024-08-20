#pragma once

#include <string>
#include <memory>
#include <variant>
#include <map>
#include <unordered_map>
#include <optional>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture2D.h"

namespace Engine {
	class MaterialInstance;

	using UniformValue = std::variant<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

	class BaseMaterial {
	public:
		virtual ~BaseMaterial() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetShader(std::shared_ptr<Shader> shader) = 0;
		virtual Shader& GetShader() = 0;
		virtual const Shader& GetShader() const = 0;

		virtual void AddTexture(std::shared_ptr<BaseTexture> texture, const std::string& uniformName, uint32_t bindingPoint) = 0;
		virtual void UpdateTexture(const std::string& uniformName, std::shared_ptr<BaseTexture> newTexture) = 0;
		virtual BaseTexture& GetTexture(const std::string& name) = 0;
		virtual uint32_t GetTextureCount() const = 0;

		virtual void SetUniform(const std::string& name, bool value) = 0;
		virtual void SetUniform(const std::string& name, int value) = 0;
		virtual void SetUniform(const std::string& name, float value) = 0;
		virtual void SetUniform(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::mat2& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::mat3& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::mat4& value) = 0;

		virtual std::optional<UniformValue> GetUniform(const std::string& name) const = 0;
		virtual std::vector<std::string> GetUniformNames() const = 0;
	};

	class Material : public BaseMaterial, public std::enable_shared_from_this<Material> {
	public:
		Material(std::shared_ptr<Shader> shader);
		~Material() override = default;

		void Bind() const override;
		void Unbind() const override;

		void SetShader(std::shared_ptr<Shader> shader) override;
		Shader& GetShader() override { return *_shader; }
		const Shader& GetShader() const override { return *_shader; }

		void AddTexture(std::shared_ptr<BaseTexture> texture, const std::string& uniformName, uint32_t bindingPoint) override;
		void UpdateTexture(const std::string& uniformName, std::shared_ptr<BaseTexture> newTexture) override;
		BaseTexture& GetTexture(const std::string& name) override;
		uint32_t GetTextureCount() const override;

		void SetUniform(const std::string& name, bool value) override;
		void SetUniform(const std::string& name, int value) override;
		void SetUniform(const std::string& name, float value) override;
		void SetUniform(const std::string& name, const glm::vec2& value) override;
		void SetUniform(const std::string& name, const glm::vec3& value) override;
		void SetUniform(const std::string& name, const glm::vec4& value) override;
		void SetUniform(const std::string& name, const glm::mat2& value) override;
		void SetUniform(const std::string& name, const glm::mat3& value) override;
		void SetUniform(const std::string& name, const glm::mat4& value) override;

		std::optional<UniformValue> GetUniform(const std::string& name) const override;
		std::vector<std::string> GetUniformNames() const override;

		std::shared_ptr<MaterialInstance> CreateInstance();
	public:
		struct Uniform {
			UniformValue value;
			bool isSampler;
		};
	private:
		std::shared_ptr<Shader> _shader;
		std::map<std::string, Uniform> _uniformMap;
		std::unordered_map<uint32_t, std::shared_ptr<BaseTexture>> _textureMap;

		friend class MaterialInstance;
	};

	class MaterialInstance : public BaseMaterial {
	public:
		MaterialInstance(std::shared_ptr<Material> baseMaterial);
		~MaterialInstance() override = default;

		void Bind() const override;
		void Unbind() const override;

		void SetShader(std::shared_ptr<Shader> shader) override;
		Shader& GetShader() override;
		const Shader& GetShader() const override;

		void AddTexture(std::shared_ptr<BaseTexture> texture, const std::string& uniformName, uint32_t bindingPoint) override;
		void UpdateTexture(const std::string& uniformName, std::shared_ptr<BaseTexture> newTexture) override;
		BaseTexture& GetTexture(const std::string& name) override;
		uint32_t GetTextureCount() const override;

		void SetUniform(const std::string& name, bool value) override;
		void SetUniform(const std::string& name, int value) override;
		void SetUniform(const std::string& name, float value) override;
		void SetUniform(const std::string& name, const glm::vec2& value) override;
		void SetUniform(const std::string& name, const glm::vec3& value) override;
		void SetUniform(const std::string& name, const glm::vec4& value) override;
		void SetUniform(const std::string& name, const glm::mat2& value) override;
		void SetUniform(const std::string& name, const glm::mat3& value) override;
		void SetUniform(const std::string& name, const glm::mat4& value) override;

		void ResetUniform(const std::string& name);

		std::optional<UniformValue> GetUniform(const std::string& name) const override;
		std::vector<std::string> GetUniformNames() const override;

		std::shared_ptr<Material> GetBaseMaterial() const { return _baseMaterial; }
		bool IsUniformOverridden(const std::string& name) const;
		std::vector<std::string> GetOverriddenUniformNames() const;
	private:
		std::shared_ptr<Material> _baseMaterial;
		std::map<std::string, Material::Uniform> _overriddenUniforms;
		std::unordered_map<uint32_t, std::shared_ptr<BaseTexture>> _overriddenTextures;
	};
}
