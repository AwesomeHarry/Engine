#pragma once
#include <string>
#include <unordered_map>
#include "Util/FileIO.h"
#include "glm/glm.hpp"

namespace Engine {
	enum UniformType {
		Bool, Int, Float, 
		Vec2, Vec3, Vec4, 
		Mat2, Mat3, Mat4,
		Sampler2D, SamplerCube
	};

	struct UniformInfo {
		std::string name;
		uint32_t location;
		UniformType type;
	};

	class Shader {
	public:
		Shader();
		~Shader();

		void AttachVertexShader(const std::string& src);
		void AttachFragmentShader(const std::string& src);
		void AttachGeometeryShader(const std::string& src);

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
		inline const std::unordered_map<std::string, UniformInfo>& GetUniformInfos() const { return _uniformsMap; }
		inline const std::unordered_map<std::string, uint32_t>& GetUniformBlocks() const { return _uniformBlockMap; }
	private:
		uint32_t compileShader(uint32_t type, const char* source);
		void preloadUniforms();
		bool uniformExists(const std::string& name);
		int getUniformLocation(const std::string& name);
	private:
		uint32_t _id;
		std::unordered_map<std::string, UniformInfo> _uniformsMap;
		std::unordered_map<std::string, uint32_t> _uniformBlockMap;
	};

	class ShaderUtils {
	public:
		static std::shared_ptr<Shader> FromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	};
}