#pragma once
#include <string>
#include <map>
#include "Util/FileIO.h"
#include "glm/glm.hpp"

namespace Engine {
	enum class ShaderStage { Vertex, Fragment, Geometry };

	enum class UniformType {
		Bool, Int, Float,
		Vec2, Vec3, Vec4,
		Mat2, Mat3, Mat4,
		Sampler2D, SamplerCube
	};

	struct UniformInfo {
		std::string name;
		uint32_t location = 0;
		UniformType type = UniformType::Bool;
	};

	class Shader {
	public:
		Shader();
		~Shader();

		void AttachShader(ShaderStage type, const std::string& src);

		void Link();

		void Bind() const;
		void Unbind() const;

		bool SetUniform(const std::string& name, bool value);
		bool SetUniform(const std::string& name, int value);
		bool SetUniform(const std::string& name, float value);
		bool SetUniform(const std::string& name, glm::vec2 value);
		bool SetUniform(const std::string& name, glm::vec3 value);
		bool SetUniform(const std::string& name, glm::vec4 value);
		bool SetUniform(const std::string& name, glm::mat2 value);
		bool SetUniform(const std::string& name, glm::mat3 value);
		bool SetUniform(const std::string& name, glm::mat4 value);

		bool BindUniformBlock(const std::string& blockName, uint32_t bindingPoint);

		inline uint32_t GetHandle() const { return _id; }

		inline bool HasType(ShaderStage type) const { return _shaderSources.find(type) != _shaderSources.end(); }
		inline const std::string& GetShaderSource(ShaderStage type) const { return _shaderSources.at(type); }
		std::vector<ShaderStage> GetAttachedTypes() const;

		inline const std::map<std::string, UniformInfo>& GetUniformInfos() const { return _uniformsMap; }
		inline const std::map<std::string, uint32_t>& GetUniformBlocks() const { return _uniformBlockMap; }
	private:
		uint32_t compileShader(uint32_t type, const char* source);
		void preloadUniforms();
		bool uniformExists(const std::string& name);
		int getUniformLocation(const std::string& name);
	public:
		struct Utils {
			/* Depreciated */ static std::shared_ptr<Shader> FromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
			static std::shared_ptr<Shader> FromFile(const std::string& filepath);
			static std::unordered_map<ShaderStage, std::string> ParseShader(const std::string& filepath);
		};
	private:
		uint32_t _id;
		std::map<ShaderStage, std::string> _shaderSources;
		std::map<std::string, UniformInfo> _uniformsMap;
		std::map<std::string, uint32_t> _uniformBlockMap;
	};
}