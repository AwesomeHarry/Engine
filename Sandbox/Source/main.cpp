#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Application/Root.h"

#include "Project/Scene/Scene.h"
#include "Project/Scene/Entity.h"

#include "Rendering/Platform/Buffer/UniformBufferObject.h"
#include "Rendering/Platform/Texture2D.h"
#include "Rendering/Platform/TextureCubeMap.h"

#include "Util/Mesh/GltfIO.h"

#include "UI/WindowInfoUI_ImGui.h"
#include "UI/SceneHeirarchyUI_ImGui.h"
#include "UI/EntityPropertiesUI_ImGui.h"
#include "UI/MaterialUI_ImGui.h"

#include "FPSCameraController.h"
#include "OrbitCameraController.h"

#include "SRP.h"

#include "Project/Project.h"
#include "Project/AssetSystem.h"
#include "Project/Assets/ShaderAsset.h"
#include "Project/Assets/TextureAsset.h"
#include "Project/Assets/MeshAsset.h"

class SandboxLayer : public Engine::Layer {
private:
	std::unique_ptr<Engine::Project> _project;
	std::shared_ptr<Engine::Scene> _scene;
	std::shared_ptr<SRP> _standardRenderPipeline;

	std::shared_ptr<Engine::Material> _standardLit;

	Engine::Entity testEntity;
	Engine::Entity _camera;
	FPSCameraController _fpsCameraController;
	OrbitCameraController _orbitCameraController;


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

		/* Camera */
		auto skyboxTexture = Engine::Texture2D::Utils::FromFile("Resources/Skyboxes/brown_photostudio_06_4k.hdr");
		auto skyboxCubemap = Engine::TextureCubemap::Utils::FromTexture2D(skyboxTexture, Engine::TextureCubemap::Utils::Texture2DCubemapFormat::Equirectangle);

		// Irradiance Map
		std::shared_ptr<Engine::TextureCubemap> irradianceCubemap;
		{
			glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
			glm::mat4 captureViews[] = {
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
			};

		#pragma region Skybox Data
			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};
		#pragma endregion
			auto cubeVa = std::make_shared<Engine::VertexArrayObject>();
			auto skyboxVbo = std::make_shared<Engine::VertexBufferObject>(Engine::BufferUsage::Static);
			skyboxVbo->SetData(skyboxVertices, 36, {
				{ "aPos", Engine::LType::Float, 3 }
							   });
			cubeVa->AddVertexBuffer(skyboxVbo);
			cubeVa->Compute();
			auto cubeMesh = std::make_shared<Engine::Mesh>();
			cubeMesh->AddSubmesh(cubeVa);

			auto irradianceShader = Engine::Shader::Utils::FromFile("Resources/Shaders/SkyboxConvolution.vert", "Resources/Shaders/SkyboxConvolution.frag");
			auto irradianceMaterial = std::make_shared<Engine::Material>(irradianceShader);
			irradianceMaterial->SetTexture("environmentMap", skyboxCubemap);
			irradianceMaterial->SetUniform("projection", captureProjection);
			auto irradianceMaterialInstance = irradianceMaterial->CreateInstance();

			uint32_t captureSize = 32;

			auto textureSpec = Engine::TextureSpec{
				captureSize,
				captureSize,
				Engine::ImageFormat::RGB16F
			};
			irradianceCubemap = std::make_shared<Engine::TextureCubemap>(textureSpec);

			auto framebufferSpec = Engine::Framebuffer::FramebufferSpec{
				captureSize,
				captureSize,
				{ Engine::ImageFormat::RGB16F },
				true
			};
			auto captureFrambuffer = std::make_shared<Engine::Framebuffer>(framebufferSpec);

			captureFrambuffer->Bind();
			for (uint32_t i = 0; i < 6; i++) {
				//captureFrambuffer->ModifyColorAttachment(0, (Engine::TextureTarget)((int)Engine::TextureTarget::CubemapPosX + i), *cubemap);
				irradianceMaterial->SetUniform("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemap->GetInstanceID(), 0);

				Engine::RenderCommands::ClearBuffers(Engine::BufferBit::Color | Engine::BufferBit::Depth);
				Engine::RenderCommands::RenderMesh(*cubeMesh, *irradianceMaterialInstance);
			}
			captureFrambuffer->Unbind();
		}

		{
			/* Add Camera */
			_camera = _scene->CreateEntity("Camera");
			_camera.GetTransform().position.z += 5.0f;
			auto& cc = _camera.AddComponent<Engine::CameraComponent>();
			cc.renderPipeline = _standardRenderPipeline;
			cc.type = Engine::CameraType::Perspective;
			cc.farPlane = 1000000;
			cc.backgroundType = Engine::CameraComponent::BackgroundType::Skybox;

			/* Cubemap */
			cc.skyboxCubemap = skyboxCubemap;
		}

		// Camera Controls
		_fpsCameraController = FPSCameraController(_inputManager);
		_orbitCameraController = OrbitCameraController(_inputManager);
		_window->Subscribe<Engine::WindowMouseScrolledEvent>([&](const Engine::WindowMouseScrolledEvent& e) {
			_orbitCameraController.OnScroll((float)e.yOffset);
		});

		/* Create Shader & Material for test scene */
		auto pbrShader = Engine::Shader::Utils::FromFile("Resources/Shaders/PBR.glsl");
		pbrShader->BindUniformBlock("CameraData", 0);

		_standardLit = std::make_unique<Engine::Material>(pbrShader);

		// Set irradiance map natively
		_standardLit->SetTexture("irradianceMap", irradianceCubemap);

		Engine::UI::MaterialUI::CustomUniformWidgets["albedo"] = { Engine::UI::MaterialUI::WidgetType::Color };
		Engine::UI::MaterialUI::CustomUniformWidgets["roughness"] = { Engine::UI::MaterialUI::WidgetType::Drag, 0.025f, 0.0f, 1.0f };
		Engine::UI::MaterialUI::CustomUniformWidgets["metallic"] = { Engine::UI::MaterialUI::WidgetType::Drag, 0.025f, 0.0f, 1.0f };
		Engine::UI::MaterialUI::CustomUniformWidgets["ao"] = { Engine::UI::MaterialUI::WidgetType::Drag, 0.025f, 0.0f, 1.0f };
		Engine::UI::MaterialUI::CustomUniformWidgets["lightColor"] = { Engine::UI::MaterialUI::WidgetType::Color };

		auto model = Engine::GltfIO::LoadModel("Resources/Models/Sphere/Sphere.gltf");
		auto mesh = Engine::GltfIO::LoadMesh(model, 0);

		{
			auto s = _scene->CreateEntity("Sphere");
			s.AddComponent<Engine::MeshFilterComponent>(mesh);
			auto& mr = s.AddComponent<Engine::MeshRendererComponent>(_standardLit);
			mr.material->SetUniform("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
		}

		/* Create Project */
		_project = std::make_unique<Engine::Project>("TestProject", std::filesystem::current_path() / "Projects/");

		/* Create Asset Bank */
		auto assetBank = std::make_shared<Engine::AssetBank>();

		/* Create Shader Asset */
		auto [shaderAsset, shaderRef] = _project->CreateAsset<Engine::ShaderAsset>("PBR", "Shaders/");
		shaderAsset->SetShaderPath("Resources/Shaders/PBR.glsl");
		shaderAsset->SetUniformBlock("CameraData", 0);

		/* Create Texture Assets */
		auto [textureAsset, textureRef] = _project->CreateAsset<Engine::TextureAsset>("UV_Test", "Textures/");
		textureAsset->SetTexturePath("Resources/Textures/UV_Test.png");
		textureAsset->SetType(Engine::TextureType::Tex2D);

		auto [cubemapAsset, cubemapRef] = _project->CreateAsset<Engine::TextureAsset>("Skybox1", "Textures/");
		cubemapAsset->SetTexturePath("Resources/Skyboxes/brown_photostudio_06_4k.hdr");
		cubemapAsset->SetType(Engine::TextureType::TexCubemap);

		/* Create Material Asset */
		auto [materialAsset, materialRef] = _project->CreateAsset<Engine::MaterialAsset>("PBR", "Materials/");
		materialAsset->SetShader(shaderRef);
		materialAsset->SetTexture("albedo", textureRef);

		/* Create Mesh Asset */
		auto [meshAsset, meshRef] = _project->CreateAsset<Engine::MeshAsset>("Sphere", "Meshes/");
		meshAsset->SetMeshPath("Resources/Models/Sphere/Sphere.gltf");
		meshAsset->SetMeshIndex(0);
	}

	void OnDetach() override {
		_project->SaveAllAssets();
	}

	void OnUpdate(float ts) override {
		/* Update anything as required */
		_orbitCameraController.OnUpdate(_camera.GetTransform(), ts);

		/* Render Axis */
		_scene->GetDebugRenderer().DrawLine({ { 0,0,0 }, { 1,0,0 }, { 1,0,0,1 } });
		_scene->GetDebugRenderer().DrawLine({ { 0,0,0 }, { 0,1,0 }, { 0,1,0,1 } });
		_scene->GetDebugRenderer().DrawLine({ { 0,0,0 }, { 0,0,1 }, { 0,0,1,1 } });

		/* Update Scene */
		_scene->UpdateScene(ts);

		_standardLit->SetUniform("camPos", _camera.GetTransform().position);

		/* Render Scene */
		_scene->RenderScene();

		/* Render UI */
		{
			//ImGui::Image((ImTextureID)(intptr_t)test_texture->GetInstanceID(), { 100,100 }, { 0,1 },{1,0});

			/* Viewport */
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::Begin("Scene");
				auto s = ImGui::GetContentRegionAvail();
				auto& mainFramebuffer = _standardRenderPipeline->GetMainFramebuffer();
				if (mainFramebuffer.GetSpecification().width != s.x ||
					mainFramebuffer.GetSpecification().height != s.y)
					_standardRenderPipeline->OnResize((uint32_t)s.x, (uint32_t)s.y);
				ImGui::Image((ImTextureID)(intptr_t)mainFramebuffer.GetColorAttachment(0)->GetInstanceID(), s, { 0,1 }, { 1,0 });
				ImGui::End();
				ImGui::PopStyleVar();
			}

			ImGui::Image((ImTextureID)(intptr_t)_standardRenderPipeline->_windowFramebuffer->GetColorAttachment(0)->GetInstanceID(), ImGui::GetContentRegionAvail(), { 0,1 }, { 1,0 });
			//ImGui::Image((ImTextureID)(intptr_t)_standardRenderPipeline->GetMainFramebuffer().GetDepthAttachment()->GetID(), ImGui::GetContentRegionAvail(), { 0,1 }, { 1,0 });

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
