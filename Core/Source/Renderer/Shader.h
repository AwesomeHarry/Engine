#pragma once
#include <string>
#include <unordered_map>
#include "Util/FileIO.h"

namespace Engine {
	class Shader {
	public:
		Shader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		inline uint32_t GetHandle() const { return _id; }

		bool SetUniform(const std::string& name, int value);
		bool SetUniform(const std::string& name, float value);

		static std::shared_ptr<Shader> FromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	private:
		void loadShaders(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
		uint32_t compileShader(uint32_t type, const char* source);
		void linkShaders(uint32_t shaderProgram, uint32_t vertexShader, uint32_t fragmentShader);
		void preloadUniforms();
		bool findUniform(const std::string& name);
	private:
		uint32_t _id;
		std::unordered_map<std::string, uint32_t> _uniformsMap;
	};
}