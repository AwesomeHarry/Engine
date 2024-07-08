#pragma once

#include "BaseComponent.h"
#include "Logging/Logging.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


namespace Engine {
	class Entity;
	class Mesh;
	class Shader;

	class VertexArrayObject;
	class VertexBufferObject;

	struct DebugShapeManager : BaseComponent {
	public:
		struct PointSpec { glm::vec3 pos; glm::vec4 color; };
		std::vector<PointSpec> pointData;

		std::shared_ptr<VertexArrayObject> pointInfoVao;
		std::shared_ptr<VertexBufferObject> pointInfoVbo;
		std::shared_ptr<Shader> pointShader;

		void DrawPoint(const PointSpec& spec);

		struct LineSpec { glm::vec3 from, to; glm::vec4 color; };
		std::vector<LineSpec> lineData;

		std::shared_ptr<VertexArrayObject> lineInfoVao;
		std::shared_ptr<VertexBufferObject> lineInfoVbo;
		std::shared_ptr<Shader> lineShader;

		void DrawLine(const LineSpec& spec);

		struct QuadSpec { glm::vec3 p1, p2, p3, p4; glm::vec4 color; };
		std::vector<QuadSpec> quadData;

		std::shared_ptr<VertexArrayObject> quadInfoVao;
		std::shared_ptr<VertexBufferObject> quadInfoVbo;
		std::shared_ptr<Shader> quadShader;

		void DrawQuad(const QuadSpec& spec);

		void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);

		void Clear();

		virtual void OnComponentAdded(Entity& entity) override;
	private:
		void initialize();
	};
}
