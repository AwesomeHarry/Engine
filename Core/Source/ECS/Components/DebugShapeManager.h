#pragma once
#include "BaseComponent.h"
#include "Logging/Logging.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Rendering/Platform/Mesh.h"

namespace Engine {
	struct DebugShapeSpec {
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::vec4 color = glm::vec4(1.0f);
	};

	struct DebugShapeManager : BaseComponent {
		using DebugShapeCall = std::pair<std::shared_ptr<Mesh>, DebugShapeSpec>;
	public:
		std::vector<DebugShapeCall> drawCalls;

		void AddDebugMeshTemplate(std::string shapeType, std::shared_ptr<Mesh> meshTemplate) {
			_meshDb.emplace(shapeType, meshTemplate);
		}

		void Draw(std::string type, const DebugShapeSpec& spec) {
			auto it = _meshDb.find(type);
			if (it == _meshDb.end()) {
				ENGINE_ERROR("Debug mesh of type {} doesn't exist", type);
				return;
			}
			drawCalls.push_back(std::make_pair(it->second, spec));
		}

		void DrawTriangle(const DebugShapeSpec& spec) {
			Draw("Triangle", spec);
		}

		void DrawQuad(const DebugShapeSpec& spec) {
			Draw("Quad", spec);
		}

		void DrawCube(glm::vec4 color) {
			// Bottom Face
			Draw("Quad", { glm::vec3(0.0f,-0.5f,0.0f),glm::vec3(90.0f,0.0f,0.0f),glm::vec3(1.0f),color });
			// Top Face
			Draw("Quad", { glm::vec3(0.0f,0.5f,0.0f),glm::vec3(-90.0f,0.0f,0.0f),glm::vec3(1.0f),color });
			// Left Face
			Draw("Quad", { glm::vec3(-0.5f,0.0f,0.0f),glm::vec3(0.0f,-90.0f,0.0f),glm::vec3(1.0f),color });
			// Right Face
			Draw("Quad", { glm::vec3(0.5f,0.0f,0.0f),glm::vec3(0.0f,90.0f,0.0f),glm::vec3(1.0f),color });
			// Front Face
			Draw("Quad", { glm::vec3(0.0f,0.0f,0.5f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f),color });
			// Back Face
			Draw("Quad", { glm::vec3(0.0f,0.0f,-0.5f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f),color });
		}

		void DrawCircle(const DebugShapeSpec& spec) {
			Draw("Circle", spec);
		}

		virtual void OnComponentAdded(Entity& entity) override {
			addDefaultShapes();
		};
	private:
		void addDefaultShapes() {
			// Triangle
			{
				std::shared_ptr<Engine::VertexArrayObject> triangleVA;
				{
				#pragma region Data

					float vertices[] = {
						/*  |POS|  */
						-0.5f, -0.5f, 0.0f,
						 0.5f, -0.5f, 0.0f,
						 0.0f,  0.5f, 0.0f
					};

					uint16_t indices[] = {
						0, 1, 2
					};

				#pragma endregion

					// Create vertex array using raw data
					auto vertexBuffer = std::make_shared<Engine::VertexBufferObject>();
					vertexBuffer->SetData(vertices, 3, {
						{ "POSITION", Engine::LType::Float, 3}
										  });

					auto indexBuffer = std::make_shared<Engine::IndexBufferObject>();
					indexBuffer->SetData(indices, Engine::LType::UnsignedShort, 3);

					triangleVA = std::make_shared<Engine::VertexArrayObject>();
					triangleVA->AddVertexBuffer(vertexBuffer);
					triangleVA->SetIndexBuffer(indexBuffer);
					triangleVA->Compute();
					triangleVA->SetDrawMode(DrawMode::Triangles);
				}

				auto mesh = std::make_shared<Engine::Mesh>("Triangle");
				mesh->AddSubmesh(triangleVA);
				AddDebugMeshTemplate("Triangle", mesh);
			}

			// Unit Quad
			{
				std::shared_ptr<Engine::VertexArrayObject> quadVA;
				{
				#pragma region Data

					float vertices[] = {
						/*  |POS|  */
							-0.5,-0.5, 0,
							 0.5,-0.5, 0,
							-0.5, 0.5, 0,
							 0.5, 0.5, 0
					};

				#pragma endregion

					// Create vertex array using raw data
					auto vertexBuffer = std::make_shared<Engine::VertexBufferObject>();
					vertexBuffer->SetData(vertices, 4, {
						{ "POSITION", Engine::LType::Float, 3}
										  });

					quadVA = std::make_shared<Engine::VertexArrayObject>();
					quadVA->AddVertexBuffer(vertexBuffer);
					quadVA->Compute();
					quadVA->SetDrawMode(DrawMode::TriangleStrip);
				}

				auto mesh = std::make_shared<Engine::Mesh>("Quad");
				mesh->AddSubmesh(quadVA);
				AddDebugMeshTemplate("Quad", mesh);
			}

			// Circle
			{
				std::shared_ptr<Engine::VertexArrayObject> circleVA;
				{
				#pragma region Data

					constexpr int segments = 36;
					float vertices[segments * 3 + 3];
					uint16_t indices[segments + 2];

					vertices[0] = 0.0f; // Center vertex
					vertices[1] = 0.0f;
					vertices[2] = 0.0f;

					for (int i = 0; i < segments; ++i) {
						float angle = 2.0f * 3.1415926535 * i / segments;
						vertices[3 + i * 3] = 0.5f * cos(angle);
						vertices[3 + i * 3 + 1] = 0.5f * sin(angle);
						vertices[3 + i * 3 + 2] = 0.0f;
					}

					for (int i = 0; i <= segments; ++i) {
						indices[i] = i;
					}
					indices[segments + 1] = 1;

				#pragma endregion

					// Create vertex array using raw data
					auto vertexBuffer = std::make_shared<Engine::VertexBufferObject>();
					vertexBuffer->SetData(vertices, segments + 1, {
						{ "POSITION", Engine::LType::Float, 3}
										  });

					auto indexBuffer = std::make_shared<Engine::IndexBufferObject>();
					indexBuffer->SetData(indices, Engine::LType::UnsignedShort, segments + 2);

					circleVA = std::make_shared<Engine::VertexArrayObject>();
					circleVA->AddVertexBuffer(vertexBuffer);
					circleVA->SetIndexBuffer(indexBuffer);
					circleVA->Compute();
					circleVA->SetDrawMode(DrawMode::TriangleFan);
				}

				auto mesh = std::make_shared<Engine::Mesh>("Circle");
				mesh->AddSubmesh(circleVA);
				AddDebugMeshTemplate("Circle", mesh);
			}

			// Line
			{
				std::shared_ptr<Engine::VertexArrayObject> lineVA;
				{
				#pragma region Data

					float vertices[] = {
						/*  |POS|  */
						-0.5f, 0.0f, 0.0f,
						 0.5f, 0.0f, 0.0f
					};

					uint16_t indices[] = {
						0, 1
					};

				#pragma endregion

					// Create vertex array using raw data
					auto vertexBuffer = std::make_shared<Engine::VertexBufferObject>();
					vertexBuffer->SetData(vertices, 2, {
						{ "POSITION", Engine::LType::Float, 3}
										  });

					auto indexBuffer = std::make_shared<Engine::IndexBufferObject>();
					indexBuffer->SetData(indices, Engine::LType::UnsignedShort, 2);

					lineVA = std::make_shared<Engine::VertexArrayObject>();
					lineVA->AddVertexBuffer(vertexBuffer);
					lineVA->SetIndexBuffer(indexBuffer);
					lineVA->Compute();
					lineVA->SetDrawMode(DrawMode::Lines);
				}

				auto mesh = std::make_shared<Engine::Mesh>("Line");
				mesh->AddSubmesh(lineVA);
				AddDebugMeshTemplate("Line", mesh);
			}
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshDb;
	};
}