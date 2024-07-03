#pragma once
#include <string>
#include <unordered_map>
#include "Util/FileIO.h"
#include "glm/glm.hpp"

namespace Engine {
	class Shader {
	public:
		Shader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

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
		inline const std::unordered_map<std::string, uint32_t>& GetUniforms() const { return _uniformsMap; }
		inline const std::unordered_map<std::string, uint32_t>& GetUniformBlocks() const { return _uniformBlockMap; }
	private:
		void loadShaders(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
		uint32_t compileShader(uint32_t type, const char* source);
		void linkShaders(uint32_t shaderProgram, uint32_t vertexShader, uint32_t fragmentShader);
		void preloadUniforms();
		bool findUniform(const std::string& name);
	private:
		uint32_t _id;
		std::unordered_map<std::string, uint32_t> _uniformsMap;
		std::unordered_map<std::string, uint32_t> _uniformBlockMap;
	};

	class ShaderUtils {
	public:
		static std::shared_ptr<Shader> FromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	};
}