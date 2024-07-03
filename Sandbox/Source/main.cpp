#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "Core/Root.h"

#include "ECS/Scene/Scene.h"
#include "ECS/Scene/Entity.h"

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/UniformBufferObject.h"

#include "Util/Mesh/GltfIO.h"

#include "UI/WindowInfoUI_ImGui.h"
#include "UI/SceneHeirarchyUI_ImGui.h"
#include "UI/EntityPropertiesUI_ImGui.h"

#pragma region Shader Source

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec4 aTan;
layout(location = 3) in vec2 aTex;
layout (std140) uniform CameraData {
	mat4 projection;
	mat4 view;
};
uniform mat4 model;
out vec3 pos;
out vec3 nor;
out vec4 tan;
out vec2 tex;
void main()
{
	tan=aTan;
	tex=aTex;
    nor = mat3(transpose(inverse(model))) * aNor;
    pos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(pos, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec3 pos;
in vec3 nor;
in vec4 tan;
in vec2 tex;

void main()
{
    //FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    FragColor = vec4(nor, 1.0);
}
)glsl";

#pragma endregion

class SandboxLayer : public Engine::Layer {
private:
	std::shared_ptr<Engine::Scene> _scene;

	float _color[3] = { 0.06f, 0.06f, 0.06f };
	std::shared_ptr<Engine::Shader> _shader;

	Engine::Entity _monkeh;

	bool _renderWireframe = false;
public:
	void OnAttach() override {
		/* Create Scene */
		_scene = std::make_shared<Engine::Scene>();

		/* Create Shader */
		auto shader = std::make_shared<Engine::Shader>(vertexShaderSource, fragmentShaderSource);
		shader->BindUniformBlock("CameraData", 0);

		{
			/* Create Manual Mesh */
			auto mesh = std::make_shared<Engine::Mesh>("Quad");
			{
			#pragma region Data

				glm::vec3 vertices[] = {
					{-0.5f,	-0.5f,	0},
					{ 0.5f,	-0.5f,	0},
					{-0.5f,	 0.5f,	0},
					{ 0.5f,	 0.5f,	0}
				};

				glm::vec2 texCoords[] = {
					{0,0},
					{1,0},
					{0,1},
					{1,1}
				};

				unsigned short indices[] = {
					0, 1, 2,
					2, 1, 3
				};

			#pragma endregion

				// Create vertex array using raw data
				std::shared_ptr<Engine::VertexArrayObject> vertexArray;
				auto vertexBuffer = std::make_shared<Engine::VertexBufferObject>();
				vertexBuffer->SetData(vertices, 4, {
					{ "POSITION", Engine::LType::Float, 3},
									  });

				auto texcoordBuffer = std::make_shared<Engine::VertexBufferObject>();
				texcoordBuffer->SetData(texCoords, 4, {
					{ "TEXCOORDS", Engine::LType::Float, 2 }
										});

				auto indexBuffer = std::make_shared<Engine::IndexBufferObject>();
				indexBuffer->SetData(indices, Engine::LType::UnsignedShort, 6);

				vertexArray = std::make_shared<Engine::VertexArrayObject>();
				vertexArray->AddVertexBuffer(vertexBuffer);
				vertexArray->SetIndexBuffer(indexBuffer);
				vertexArray->Compute();

				mesh->AddSubmesh(vertexArray);
			}

			/* Add mesh to scene */
			//Engine::Entity entity = _scene->CreateEntity("Manual Mesh");
			//entity.AddComponent<Engine::MeshFilterComponent>(mesh);
			//entity.AddComponent<Engine::MeshRendererComponent>(shader);
		}

		{
			/* Import Gltf Mesh */
			auto gltfMesh = std::make_shared<Engine::Mesh>("Monkeh");
			{
				auto model = Engine::GltfIO::LoadFile("Resources/Suzanne/glTF/Suzanne.gltf");

				const auto& mesh = model.meshes[0];
				for (const auto& primitive : mesh.primitives) {
					auto vertexArray = Engine::GltfIO::LoadPrimitive(model, primitive);
					gltfMesh->AddSubmesh(vertexArray);
				}
			}

			Engine::Entity entity = _scene->CreateEntity("GLTF Mesh");
			entity.AddComponent<Engine::MeshFilterComponent>(gltfMesh);
			entity.AddComponent<Engine::MeshRendererComponent>(shader);

			_monkeh = entity;
		}

		{
			/* Add Camera */
			Engine::Entity camera = _scene->CreateEntity("Camera");
			camera.GetTransform().position.z -= 5.0f;
			auto& cc = camera.AddComponent<Engine::CameraComponent>();
			{
				float width = (float)_window->GetWidth();
				float height = (float)_window->GetHeight();

				float aspectRatio = width / height;
				float orthoSize = 1.0f * std::max(width, height) / height;

				float left = -orthoSize * aspectRatio;
				float right = orthoSize * aspectRatio;
				float bottom = -orthoSize;
				float top = orthoSize;

				cc.SetOrthographic(left, right, bottom, top, cc.nearPlane, cc.farPlane);
			}

			_window->Subscribe<Engine::WindowResizeEvent>([&](const Engine::WindowResizeEvent& e) {
				float width = (float)e.GetWidth();
				float height = (float)e.GetHeight();

				float aspectRatio = width / height;
				float orthoSize = 1.0f * std::max(width, height) / height;

				float left = -orthoSize * aspectRatio;
				float right = orthoSize * aspectRatio;
				float bottom = -orthoSize;
				float top = orthoSize;

				cc.SetOrthographic(left, right, bottom, top, cc.nearPlane, cc.farPlane);
			});
		}
	}

	int initialTab = 0;

	void OnUpdate(float ts) override {
		/* Update anything as required */
		{
			auto& transform = _monkeh.GetTransform();
			transform.rotation.x += 20.0f * ts;
			transform.rotation.y += 30.0f * ts;
			transform.rotation.z += 40.0f * ts;
		}

		/* Update Scene */
		_scene->UpdateScene(ts);

		/* Render Scene */
		_renderManager->RenderScene(*_scene);

		/* Render UI */
		ImGui::Begin("Window");
		Engine::WindowInfoUI_ImGui::RenderUI(*_window);
		ImGui::End();

		ImGui::Begin("Properties");
		Engine::EntityPropertiesUI_ImGui::RenderUI(Engine::SceneHeirarchyUI_ImGui::GetSelectedEntity());
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Clear Color", &_color[0]);
		_renderManager->SetClearColor(_color[0], _color[1], _color[2], 1);
		if (ImGui::Checkbox("Wireframe", &_renderWireframe)) {
			_renderManager->SetWireframeMode(_renderWireframe ? Engine::WireframeMode::Line : Engine::WireframeMode::Fill);
		}
		ImGui::Text("Delta Time: %.6fms", ts);
		ImGui::Text("FPS: %.1f", 1.0f / ts);
		ImGui::End();

		ImGui::Begin("Scene Info");
		Engine::SceneHeirarchyUI_ImGui::RenderUI(*_scene);
		ImGui::End();
	}
};

class SRP : public Engine::IRenderPipeline {
private:
	struct CameraData { glm::mat4 projection; glm::mat4 view; };
	CameraData _cameraData;
	std::shared_ptr<Engine::UniformBufferObject> _cameraDataUbo;
public:
	virtual void Initialize() {
		_cameraData.projection = glm::ortho(-1, 1, -1, 1);
		_cameraDataUbo = std::make_shared<Engine::UniformBufferObject>(sizeof(CameraData), 0);
	}

	virtual void RenderScene(const Engine::Scene& scene) override {
		//Engine::RenderCommands::SetClearColor(currentCamera.GetClearColor());
		Engine::RenderCommands::ClearBuffers({ Engine::BufferBit::Color, Engine::BufferBit::Depth });

		/* Set render targets from cameras */
		{}

		/* RenderOpaqueObjects(scene, camera); */
		/* RenderTransparentObjects(scene, camera); */

		/* ApplyPostProcessing(); */

		auto& reg = scene.GetRegistry();

		{
			auto view = reg.view<const Engine::TransformComponent, const Engine::CameraComponent>();
			for (auto entity : view) {
				auto& transform = view.get<Engine::TransformComponent>(entity);
				auto& camera = view.get<Engine::CameraComponent>(entity);

				if (camera.isMainCamera) {
					_cameraData.projection = camera.GetProjectionMatrix();
					_cameraData.view = camera.CalculateViewMatrix(transform);
				}


			}
		}

		_cameraDataUbo->Bind();
		_cameraDataUbo->SetData(&_cameraData, sizeof(CameraData), 0);

		{
			auto view = reg.view<const Engine::TransformComponent, const Engine::MeshFilterComponent, const Engine::MeshRendererComponent>();
			for (auto entity : view) {
				auto& filter = view.get<Engine::MeshFilterComponent>(entity);
				auto& renderer = view.get<Engine::MeshRendererComponent>(entity);
				auto& transform = view.get<Engine::TransformComponent>(entity);

				auto& shader = *renderer.shader;

				glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), transform.position);
				modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMatrix = glm::scale(modelMatrix, transform.scale);

				shader.SetUniform("model", modelMatrix);

				Engine::RenderCommands::RenderMesh(*filter.mesh, shader);
			}
		}
	}
};

int main(int argc, char** argv) {
	auto root = std::make_unique<Engine::Root>();

	Engine::WindowSpec windowSpec{};
	windowSpec.Title = "Sandbox";
	windowSpec.Width = 1280;
	windowSpec.Height = 720;

	Engine::GraphicsContext graphicsContext{};
	graphicsContext.EnableDepthTest = true;
	graphicsContext.CullBack = true;

	if (!root->Initialize(windowSpec, graphicsContext))
		return 1;

	auto& renderer = root->GetRenderer();
	renderer.SetPipeline<SRP>();

	auto layer = std::make_shared<SandboxLayer>();
	root->PushLayer(layer);

	root->Run();
	root->Shutdown();
	return 0;
}
