#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

using namespace Engine;

Shader::Shader() {
	_id = glCreateProgram();
}

Shader::~Shader() {
	glDeleteProgram(_id);
}

void Shader::AttachVertexShader(const std::string& src) {
	GLuint shader = compileShader(GL_VERTEX_SHADER, src.c_str());
	glAttachShader(_id, shader);
	glDeleteShader(shader);
}

void Shader::AttachFragmentShader(const std::string& src) {
	GLuint shader = compileShader(GL_FRAGMENT_SHADER, src.c_str());
	glAttachShader(_id, shader);
	glDeleteShader(shader);
}

void Shader::AttachGeometeryShader(const std::string& src) {
	GLuint shader = compileShader(GL_GEOMETRY_SHADER, src.c_str());
	glAttachShader(_id, shader);
	glDeleteShader(shader);
}

void Shader::Link() {
	glLinkProgram(_id);

	GLint success;
	glGetProgramiv(_id, GL_LINK_STATUS, &success);
	if (!success) {
		GLint infoLogLength;
		glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::string infoLog(infoLogLength, ' ');
		glGetProgramInfoLog(_id, infoLogLength, &infoLogLength, &infoLog[0]);
		ENGINE_ERROR("Shader linking failed: {}", infoLog);
		return;
	}

	preloadUniforms();
}

void Shader::Bind() const {
	glUseProgram(_id);
}

void Shader::Unbind() const {
	glUseProgram(0);
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

	auto shader = std::make_shared<Shader>();
	shader->AttachVertexShader(vertexShaderSource);
	shader->AttachFragmentShader(fragmentShaderSource);
	shader->Link();
	return shader;
}
