#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Application/Root.h"

#include "ECS/Scene/Scene.h"
#include "ECS/Scene/Entity.h"

#include "Rendering/Platform/Mesh.h"
#include "Rendering/Platform/Shader.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubeMap.h"
#include "Rendering/Platform/Buffer/UniformBufferObject.h"

#include "Util/Mesh/GltfIO.h"

#include "UI/WindowInfoUI_ImGui.h"
#include "UI/SceneHeirarchyUI_ImGui.h"
#include "UI/EntityPropertiesUI_ImGui.h"

#include "FPSCameraController.h"
#include "OrbitCameraController.h"

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

uniform sampler2D tex0;

in vec3 pos;
in vec3 nor;
in vec4 tan;
in vec2 tex;

void main()
{
    //FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    //FragColor = vec4(normalize(nor), 1.0);
	FragColor = texture(tex0, tex);
}
)glsl";

#pragma endregion

const char* textureVertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
layout (std140) uniform CameraData {
	mat4 projection;
	mat4 view;
};
uniform mat4 model;
out vec2 uv;
void main() {
    vec3 pos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(pos, 1.0);
	uv = aTex;
}
)";

const char* textureFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D tex0;
uniform float exposure = 1.0;
void main() {
	vec3 hdrColor = texture(tex0, uv).rgb;
	vec3 exposed = vec3(1.0) - exp(-hdrColor * exposure);
	vec3 mapped = pow(exposed, vec3(1.0 / 2.2));
	FragColor = vec4(mapped, 1.0);
}
)";

class SandboxLayer : public Engine::Layer {
private:
	std::shared_ptr<Engine::Scene> _scene;
	std::shared_ptr<SRP> _standardRenderPipeline;

	Engine::Entity _monkeh;
	Engine::Entity _camera;
	FPSCameraController _fpsCameraController;
	OrbitCameraController _orbitCameraController;

	Engine::Entity _cube;
	std::shared_ptr<Engine::Shader> textureShader;

	bool _renderWireframe = false;
public:
	void OnAttach() override {
		/* Create Scene */
		_scene = std::make_shared<Engine::Scene>();

		/* Create Standard Render Pipeline */
		_standardRenderPipeline = std::make_shared<SRP>();
		{
			Engine::Framebuffer::FramebufferSpec fbSpec;
			fbSpec.width = 1280;
			fbSpec.height = 720;
			fbSpec.attachments = { Engine::ImageFormat::RGB8 };
			fbSpec.includeDepthStencil = true;

			auto mainFramebuffer = std::make_shared<Engine::Framebuffer>(fbSpec);
			_standardRenderPipeline->Initialize(mainFramebuffer);
		}

		/* Create Shader for test scene */
		auto shader = std::make_shared<Engine::Shader>();
		shader->AttachVertexShader(vertexShaderSource);
		shader->AttachFragmentShader(fragmentShaderSource);
		shader->Link();
		shader->BindUniformBlock("CameraData", 0);

		auto material = std::make_shared<Engine::Material>(shader);

		textureShader = std::make_shared<Engine::Shader>();
		textureShader->AttachVertexShader(textureVertexShaderSrc);
		textureShader->AttachFragmentShader(textureFragmentShaderSrc);
		textureShader->Link();
		textureShader->BindUniformBlock("CameraData", 0);
		textureShader->SetUniform("tex0", 0);

		auto textureMaterial = std::make_shared<Engine::Material>(textureShader);

		auto texture = Engine::Texture2D::Utils::FromFile("Resources/Skyboxes/quarry_cloudy_4k.hdr");
		textureMaterial->AddTexture(texture, 0);

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
				vertexArray->AddVertexBuffer(texcoordBuffer);
				vertexArray->SetIndexBuffer(indexBuffer);
				vertexArray->Compute();

				mesh->AddSubmesh(vertexArray);
			}

			/* Add mesh to scene */
			Engine::Entity entity = _scene->CreateEntity("Manual Mesh");
			entity.AddComponent<Engine::MeshFilterComponent>(mesh);
			entity.AddComponent<Engine::MeshRendererComponent>(textureMaterial);
		}

		/* Gltf Mesh */
		{
			/* Import Gltf Mesh */
			auto gltfMesh = std::make_shared<Engine::Mesh>("Monkeh");
			auto model = Engine::GltfIO::LoadFile("Resources/Stanford_Dragon/LowRes.gltf");
			//auto model = Engine::GltfIO::LoadFile("Resources/Sponza_Complex/sponza_complex.gltf");

			const auto& mesh = model.meshes[0];
			for (const auto& primitive : mesh.primitives) {
				auto vertexArray = Engine::GltfIO::LoadPrimitive(model, primitive);
				gltfMesh->AddSubmesh(vertexArray);
			}

			auto s = model.nodes[0].scale;
			glm::vec3 scale = s.size() > 0 ? glm::vec3(s[0], s[1], s[2]) : glm::vec3(1.0f);

			Engine::Entity entity = _scene->CreateEntity("GLTF Mesh");
			entity.GetTransform().scale = scale;
			//entity.AddComponent<Engine::MeshFilterComponent>(gltfMesh);
			entity.AddComponent<Engine::MeshRendererComponent>(material);

			//auto& bb = entity.AddComponent<Engine::BoundingBoxComponent>();
			//bb.GrowToInclude(*gltfMesh);

			_monkeh = entity;
		}

		/* Camera */
		{
			/* Cubemap */
			Engine::CubeMapPaths paths;
			paths.positiveX = "Resources/Skyboxes/Cloudy/px.hdr";
			paths.negativeX = "Resources/Skyboxes/Cloudy/nx.hdr";
			paths.positiveY = "Resources/Skyboxes/Cloudy/py.hdr";
			paths.negativeY = "Resources/Skyboxes/Cloudy/ny.hdr";
			paths.positiveZ = "Resources/Skyboxes/Cloudy/pz.hdr";
			paths.negativeZ = "Resources/Skyboxes/Cloudy/nz.hdr";
			auto cubemap = Engine::TextureCubeMap::Utils::FromFile(paths.GetArray());

			/* Add Camera */
			_camera = _scene->CreateEntity("Camera");
			_camera.GetTransform().position.z += 5.0f;
			auto& cc = _camera.AddComponent<Engine::CameraComponent>();
			cc.renderPipeline = _standardRenderPipeline;
			cc.type = Engine::CameraType::Perspective;
			cc.farPlane = 1000000;
			cc.backgroundType = Engine::CameraComponent::BackgroundType::Skybox;
			cc.skyboxCubemap = cubemap;
			/*_window->Subscribe<Engine::WindowResizeEvent>([&](const Engine::WindowResizeEvent& e) {
				cc.aspectRatio = _window->GetAspect();
			});*/
		}

		_fpsCameraController = FPSCameraController(_inputManager);
		_orbitCameraController = OrbitCameraController(_inputManager);
		_window->Subscribe<Engine::WindowMouseScrolledEvent>([&](const Engine::WindowMouseScrolledEvent& e) {
			_orbitCameraController.OnScroll((float)e.yOffset);
		});
	}

	void OnUpdate(float ts) override {
		/* Update anything as required */
		{
			_orbitCameraController.OnUpdate(_camera.GetTransform(), ts);
		}

		/* Render Axis */
		_scene->GetDebugRenderer().DrawLine({ { 0,0,0 }, { 1,0,0 }, { 1,0,0,1 } });
		_scene->GetDebugRenderer().DrawLine({ { 0,0,0 }, { 0,1,0 }, { 0,1,0,1 } });
		_scene->GetDebugRenderer().DrawLine({ { 0,0,0 }, { 0,0,1 }, { 0,0,1,1 } });

		static float exposure = 1.0f;
		ImGui::DragFloat("Exposure", &exposure, 0.1f, 0.1f, 10.0f);
		textureShader->SetUniform("exposure", exposure);

		/* Update Scene */
		_scene->UpdateScene(ts);

		/* Render Bounding Box */
		//auto& bb = _monkeh.GetComponent<Engine::BoundingBoxComponent>();
		//_scene->GetDebugRenderer().DrawCube(bb.GetCenter(), bb.GetSize(), glm::vec4(1, 0, 0, 1), true);
		//_scene->GetDebugRenderer().DrawCube(bb.GetCenter(), bb.GetSize(), glm::vec4(1, 1, 1, 0.2), false);
		//_scene->GetDebugRenderer().DrawPoint({ {bb.min.x,bb.min.y,bb.min.z},5.0f,{1,1,1,1} });
		//_scene->GetDebugRenderer().DrawPoint({ {bb.min.x,bb.max.y,bb.min.z},5.0f,{1,1,1,1} });
		//_scene->GetDebugRenderer().DrawPoint({ {bb.min.x,bb.min.y,bb.max.z},5.0f,{1,1,1,1} });
		//_scene->GetDebugRenderer().DrawPoint({ {bb.min.x,bb.max.y,bb.max.z},5.0f,{1,1,1,1} });
		//_scene->GetDebugRenderer().DrawPoint({ {bb.max.x,bb.min.y,bb.min.z},5.0f,{1,1,1,1} });
		//_scene->GetDebugRenderer().DrawPoint({ {bb.max.x,bb.max.y,bb.min.z},5.0f,{1,1,1,1} });
		//_scene->GetDebugRenderer().DrawPoint({ {bb.max.x,bb.min.y,bb.max.z},5.0f,{1,1,1,1} });
		//_scene->GetDebugRenderer().DrawPoint({ {bb.max.x,bb.max.y,bb.max.z},5.0f,{1,1,1,1} });

		/* Render Scene */
		_scene->RenderScene();

		/* Render UI */
		{
			/* Viewport */
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::Begin("Scene");
				auto s = ImGui::GetContentRegionAvail();
				auto& mainFramebuffer = _standardRenderPipeline->GetMainFramebuffer();
				if (mainFramebuffer.GetSpecification().width != (uint32_t)s.x ||
					mainFramebuffer.GetSpecification().height != (uint32_t)s.y) {
					mainFramebuffer.Resize(s.x, s.y);
				}
				ImGui::Image((ImTextureID)(intptr_t)mainFramebuffer.GetColorAttachment(0)->GetID(), s, { 0,1 }, { 1,0 });
				ImGui::End();
				ImGui::PopStyleVar();
			}

			ImGui::Image((ImTextureID)(intptr_t)_standardRenderPipeline->GetMainFramebuffer().GetDepthAttachment()->GetID(), ImGui::GetContentRegionAvail(), { 0,1 }, { 1,0 });

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
			FPSCameraControllerUI_ImGui::RenderUI(_fpsCameraController);

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

	auto layer = std::make_shared<SandboxLayer>();
	root->PushLayer(layer);

	root->Run();
	root->Shutdown();
	return 0;
}
