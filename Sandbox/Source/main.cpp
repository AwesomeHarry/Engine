#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Application/Root.h"

#include "ECS/Scene/Scene.h"
#include "ECS/Scene/Entity.h"

#include "Rendering/Platform/Mesh.h"
#include "Rendering/Platform/Shader.h"
#include "Rendering/Platform/Buffer/UniformBufferObject.h"

#include "Util/Mesh/GltfIO.h"

#include "UI/WindowInfoUI_ImGui.h"
#include "UI/SceneHeirarchyUI_ImGui.h"
#include "UI/EntityPropertiesUI_ImGui.h"

#include "CameraController.h"
#include "SRP.h"

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

	Engine::Entity _monkeh;
	Engine::Entity _camera;
	CameraController _cameraController;

	bool _renderWireframe = false;
public:
	void OnAttach() override {
		/* Create Scene */
		_scene = std::make_shared<Engine::Scene>();

		/* Create Shader */
		auto shader = std::make_shared<Engine::Shader>(vertexShaderSource, fragmentShaderSource);
		shader->BindUniformBlock("CameraData", 0);

		/* Manual Mesh */
		{
			/* Create Quad */
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

		/* Gltf Mesh */
		{
			/* Import Gltf Mesh */
			auto gltfMesh = std::make_shared<Engine::Mesh>("Monkeh");
			{
				auto model = Engine::GltfIO::LoadFile("Resources/Suzanne/glTF/Suzanne.gltf");
				//auto model = Engine::GltfIO::LoadFile("Resources/Sponza_Complex/sponza_complex.gltf");

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

		/* Camera */
		{
			/* Add Camera */
			_camera = _scene->CreateEntity("Camera");
			_camera.GetTransform().position.z -= 5.0f;
			auto& cc = _camera.AddComponent<Engine::CameraComponent>();
			cc.type = Engine::CameraType::Perspective;
			cc.farPlane = 1000000;
			/*_window->Subscribe<Engine::WindowResizeEvent>([&](const Engine::WindowResizeEvent& e) {
				cc.aspectRatio = _window->GetAspect();
			});*/
		}

		_cameraController = CameraController(_inputManager);
		_window->Subscribe<Engine::WindowMouseScrolledEvent>([&](const Engine::WindowMouseScrolledEvent& e) {
			_cameraController.OnScroll((float)e.yOffset);
		});
	}

	void OnUpdate(float ts) override {
		/* Update anything as required */
		{
			_cameraController.OnUpdate(ts);
			auto& tc = _camera.GetTransform();
			tc.position = _cameraController.GetPosition();
			tc.rotation = _cameraController.GetRotation();
			auto& cc = _camera.GetComponent<Engine::CameraComponent>();
			if (cc.type == Engine::CameraType::Perspective)
				cc.perspectiveSpec.fov = _cameraController.GetFOV();
			else
				cc.orthographicSpec.size = _cameraController.GetFOV();
		}

		/* Update Scene */
		_scene->UpdateScene(ts);

		static Engine::DebugShapeSpec spec;
		ImGui::DragFloat3("Pos", &spec.position[0], 0.1f);
		ImGui::DragFloat3("Rot", &spec.rotation[0], 0.1f);
		ImGui::DragFloat3("Sca", &spec.scale[0], 0.1f);
		ImGui::ColorEdit4("Col", &spec.color[0]);
		_scene->GetDebugRenderer().Draw("Line", spec);

		/* Render Scene */
		_renderManager->RenderScene(*_scene);

		/* Render UI */
		{
			ImGui::Begin("Window");
			Engine::WindowInfoUI_ImGui::RenderUI(*_window);
			ImGui::End();

			ImGui::Begin("Properties");
			Engine::EntityPropertiesUI_ImGui::RenderUI(Engine::SceneHeirarchyUI_ImGui::GetSelectedEntity());
			ImGui::End();

			ImGui::Begin("Settings");
			if (ImGui::Checkbox("Wireframe", &_renderWireframe)) {
				_renderManager->SetWireframeMode(_renderWireframe ? Engine::WireframeMode::Line : Engine::WireframeMode::Fill);
			}
			ImGui::Text("Delta Time: %.6fms", ts);
			ImGui::Text("FPS: %.1f", 1.0f / ts);
			glm::vec2 mp = _inputManager->GetMousePosition();
			ImGui::Text("Mouse Position: %.1f,%.1f", mp.x, mp.y);

			ImGui::Separator();
			CameraControllerUI_ImGui::RenderUI(_cameraController);

			ImGui::End();

			ImGui::Begin("Scene Info");
			Engine::SceneHeirarchyUI_ImGui::RenderUI(*_scene);
			ImGui::End();
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
	graphicsContext.EnableBlend = true;
	graphicsContext.EnableCullFace = true;
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
