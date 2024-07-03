#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

using namespace Engine;

Shader::Shader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc) {
	loadShaders(vertexShaderSrc, fragmentShaderSrc);
	preloadUniforms();
}

Shader::~Shader() {
	glDeleteProgram(_id);
}

void Shader::Bind() const {
	glUseProgram(_id);
}

void Shader::Unbind() const {
	glUseProgram(0);
}

void Shader::loadShaders(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc) {
	// Compile the shaders
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc.c_str());
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc.c_str());

	// Create the shader program
	_id = glCreateProgram();

	// Attach the shaders to the program
	linkShaders(_id, vertexShader, fragmentShader);

	// Delete the shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

uint32_t Shader::compileShader(uint32_t type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	// Check for compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		ENGINE_ERROR("ERROR::SHADER::COMPILATION_FAILED\n{}", infoLog);
	}

	return shader;
}

void Shader::linkShaders(uint32_t shaderProgram, uint32_t vertexShader, uint32_t fragmentShader) {
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		ENGINE_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", infoLog);
	}
}

void Shader::preloadUniforms() {
	int32_t numUniforms = 0;
	glGetProgramiv(_id, GL_ACTIVE_UNIFORMS, &numUniforms);
	int32_t maxUniformNameLength = 0;
	glGetProgramiv(_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);

	std::vector<char> uniformNameBuffer(maxUniformNameLength);
	for (size_t i = 0; i < numUniforms; i++) {
		GLsizei nameLength = 0;
		GLint size = 0;
		GLenum type = 0;
		glGetActiveUniform(_id, i, maxUniformNameLength, &nameLength, &size, &type, uniformNameBuffer.data());

		std::string uniformName(uniformNameBuffer.data(), nameLength);
		uint32_t location = glGetUniformLocation(_id, uniformName.c_str());

		_uniformsMap[uniformName] = location;
	}
}

bool Shader::findUniform(const std::string& name) {
	if (_uniformsMap.find(name) == _uniformsMap.end()) {
		ENGINE_ERROR("Uniform {} not found!", name);
		return false;
	}

	return true;
}

#define CHECK_UNIFORM_EXISTS {if (!findUniform(name)) return false; else glUseProgram(_id);}

bool Shader::SetUniform(const std::string& name, int value) {
	CHECK_UNIFORM_EXISTS;
	glUniform1i(_uniformsMap[name], value);
	return true;
}

bool Shader::SetUniform(const std::string& name, float value) {
	CHECK_UNIFORM_EXISTS;
	glUniform1f(_uniformsMap[name], value);
	return true;
}

bool Shader::SetUniform(const std::string& name, glm::vec2 value) {
	CHECK_UNIFORM_EXISTS;
	glUniform2fv(_uniformsMap[name], 1, glm::value_ptr(value));
	return true;
}

bool Shader::SetUniform(const std::string& name, glm::vec3 value) {
	CHECK_UNIFORM_EXISTS;
	glUniform3fv(_uniformsMap[name], 1, glm::value_ptr(value));
	return true;
}

bool Shader::SetUniform(const std::string& name, glm::vec4 value) {
	CHECK_UNIFORM_EXISTS;
	glUniform4fv(_uniformsMap[name], 1, glm::value_ptr(value));
	return true;
}

bool Shader::SetUniform(const std::string& name, glm::mat2 value) {
	CHECK_UNIFORM_EXISTS;
	glUniformMatrix2fv(_uniformsMap[name], 1, GL_FALSE, glm::value_ptr(value));
	return true;
}

bool Shader::SetUniform(const std::string& name, glm::mat3 value) {
	CHECK_UNIFORM_EXISTS;
	glUniformMatrix3fv(_uniformsMap[name], 1, GL_FALSE, glm::value_ptr(value));
	return true;
}

bool Shader::SetUniform(const std::string& name, glm::mat4 value) {
	CHECK_UNIFORM_EXISTS;
	glUniformMatrix4fv(_uniformsMap[name], 1, GL_FALSE, glm::value_ptr(value));
	return true;
}

bool Shader::BindUniformBlock(const std::string& blockName, uint32_t bindingPoint) {
	glUseProgram(_id);
	uint32_t blockIndex = glGetUniformBlockIndex(_id, blockName.c_str());
	if (blockIndex == GL_INVALID_INDEX) {
		ENGINE_ERROR("Uniform block '{}' not found in shader program!", blockName);
		return false;
	}
	glUniformBlockBinding(_id, blockIndex, bindingPoint);
	_uniformBlockMap[blockName] = blockIndex;
	return true;
}

/* ====================== */
/*      Shader Utils      */
/* ====================== */

std::shared_ptr<Shader> ShaderUtils::FromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
	// Read the shader source files
	std::string vertexShaderSource = FileIO::ReadFile(vertexShaderPath);
	std::string fragmentShaderSource = FileIO::ReadFile(fragmentShaderPath);

	ENGINE_TRACE("Vertex Shader Source:\n{}", vertexShaderSource);
	ENGINE_TRACE("Fragment Shader Source:\n{}", fragmentShaderSource);

	return std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);
}
