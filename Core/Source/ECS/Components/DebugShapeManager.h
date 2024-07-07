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

		virtual void OnComponentAdded(Entity& entity) override;
	private:
		void initialize();
	};
}
