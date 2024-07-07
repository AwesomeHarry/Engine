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
		std::vector<PointSpec> points;

		void DrawPoint(const PointSpec& spec);

		struct LineSpec { glm::vec3 from, to; glm::vec4 color; };
		std::vector<LineSpec> lineData;

		std::shared_ptr<VertexArrayObject> lineInfoVao;
		std::shared_ptr<VertexBufferObject> lineInfoVbo;
		std::shared_ptr<Shader> lineShader;


		void DrawLine(const LineSpec& spec);

		const Mesh& GetMesh() const { return *_meshTemplate; }
		Shader& GetPointShader() { return *_pointShader; }

		virtual void OnComponentAdded(Entity& entity) override;
	private:
		void initialize();

		std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshDb;
		std::shared_ptr<Mesh> _meshTemplate;
		std::shared_ptr<Shader> _pointShader;

	};
}
