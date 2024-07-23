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
layout(location = 2) in vec2 aTex;

layout (std140) uniform CameraData {
	mat4 projection;
	mat4 view;
};
uniform mat4 model;

out vec3 vPos;
out vec3 vNor;
out vec2 vTex;

void main()
{
	vTex = aTex;
    vNor = mat3(transpose(inverse(model))) * aNor;
    vPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * vec4(vPos, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNor;
in vec2 vTex;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 albedo;
uniform vec3 emission;
uniform float roughness;
uniform float metallic;

uniform bool useAlbedoMap;
uniform sampler2D albedoMap;

uniform bool useEmissionMap;
uniform sampler2D emissionMap;

uniform bool useNormalMap;
uniform sampler2D normalMap;

uniform bool useRoughnessMap;
uniform sampler2D roughnessMap;

const float PI = 3.1415926535;

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, vTex).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vPos);
    vec3 Q2  = dFdy(vPos);
    vec2 st1 = dFdx(vTex);
    vec2 st2 = dFdy(vTex);

    vec3 N = normalize(vNor);
    vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// GGX / Trowbridge-Reitz Normal Distribution Function
float D(float alpha, vec3 N, vec3 H)
{
	float a2 = alpha * alpha;

	float numerator = a2;

	float NdotH = max(dot(N, H), 0.0);
	float denominator = PI * pow(NdotH * NdotH * (a2 - 1.0) + 1.0, 2.0);
	denominator = max(denominator, 0.000001);

	return numerator / denominator;
}

// Schlick-Beckmann Geometry Shadowing Function
float G1(float alpha, vec3 N, vec3 X)
{
	float NdotX = max(dot(N, X), 0.0);
	float numerator = NdotX;

	float k = alpha / 2.0;
	float denominator = NdotX * (1.0 - k) + k;
	denominator = max(denominator, 0.000001);

	return numerator / denominator;
}

// Smith Model
float G(float alpha, vec3 N, vec3 V, vec3 L)
{
	return G1(alpha, N, V) * G1(alpha, N, L);
}

// Fresnel-Schlick Function
vec3 F(vec3 F0, vec3 V, vec3 H)
{
	float VdotH = max(dot(V, H), 0.0);
	return F0 + (vec3(1.0) - F0) * pow(1.0 - VdotH, 5.0);
}

void main()
{
	vec3 N;
	if (useNormalMap)
		N = GetNormalFromMap();
	else
		N = normalize(vNor);

	vec3 V = normalize(cameraPosition - vPos);
	// For directional lights
	vec3 L = normalize(lightPosition);

	vec3 H = normalize(V + L);
	
	vec3 F0 = vec3(0.04);
	vec3 fresnel = F(F0, V, H);

	vec3 Ks = fresnel;
	vec3 Kd = vec3(1.0) - Ks;

	float materialRoughness = roughness;
	float materialMetallic = metallic;
	if (useRoughnessMap) {
		materialRoughness += texture(roughnessMap, vTex).g;
		materialMetallic += texture(roughnessMap, vTex).b;
	}
	
    Kd *= 1.0 - materialMetallic;

	vec3 lambert = albedo / PI;
	if (useAlbedoMap)
		lambert *= texture(albedoMap,vTex).rgb;

	vec3 cookTorranceNumerator = D(materialRoughness, N, H) * G(materialRoughness, N, V, L) * fresnel;
	float cookTorranceDenominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
	vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

	vec3 BRDF = Kd * lambert + cookTorrance;
	
	vec3 emissionColor = emission;
	if (useEmissionMap)
		emissionColor += texture(emissionMap, vTex).rgb;

	vec3 ambientLight = vec3(0.03) * albedo;

	vec3 outgoingLight = ambientLight + emissionColor + BRDF * lightColor * max(dot(L, N), 0.0);
	FragColor = vec4(outgoingLight, 1.0);
}
)glsl";

#pragma endregion

class SandboxLayer : public Engine::Layer {
private:
	std::shared_ptr<Engine::Scene> _scene;
	std::shared_ptr<SRP> _standardRenderPipeline;

	std::shared_ptr<Engine::Material> _standardLit;

	Engine::Entity testEntity;
	Engine::Entity _camera;
	FPSCameraController _fpsCameraController;
	OrbitCameraController _orbitCameraController;

	Engine::Entity _cube;

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

		/* Create Shader & Material for test scene */
		auto shader = std::make_shared<Engine::Shader>();
		shader->AttachVertexShader(vertexShaderSource);
		shader->AttachFragmentShader(fragmentShaderSource);
		shader->Link();
		shader->BindUniformBlock("CameraData", 0);

		_standardLit = std::make_shared<Engine::Material>(shader);

		// Attach textures
		auto albedo = Engine::Texture2D::Utils::FromFile("Resources/Models/DamagedHelmet/gltf/Default_albedo.jpg", false);
		_standardLit->AddTexture(albedo, "albedoMap", 0);

		auto emission = Engine::Texture2D::Utils::FromFile("Resources/Models/DamagedHelmet/gltf/Default_emissive.jpg", false);
		_standardLit->AddTexture(emission, "emissionMap", 1);

		auto normalMap = Engine::Texture2D::Utils::FromFile("Resources/Models/DamagedHelmet/gltf/Default_normal.jpg", false);
		_standardLit->AddTexture(normalMap, "normalMap", 2);

		auto roughnessMap = Engine::Texture2D::Utils::FromFile("Resources/Models/DamagedHelmet/gltf/Default_metalRoughness.jpg", false);
		_standardLit->AddTexture(roughnessMap, "roughnessMap", 3);

		_standardLit->SetUniform("lightPosition", glm::vec3(1, 1, 0));
		_standardLit->SetUniform("lightColor", glm::vec3(1, 1, 1));
		_standardLit->SetUniform("albedo", glm::vec3(1, 1, 1));
		_standardLit->SetUniform("roughness", 0.0f);
		_standardLit->SetUniform("metallic", 0.0f);

		_standardLit->SetUniform("useAlbedoMap", true);
		_standardLit->SetUniform("useEmissionMap", true);
		_standardLit->SetUniform("useNormalMap", true);
		_standardLit->SetUniform("useRoughnessMap", true);

		/* Manual Mesh */
		{
			/* Create Quad */
			auto mesh = std::make_shared<Engine::Mesh>("Quad");
			{
			#pragma region Data

				glm::vec3 vertices[] = {
					{-0.5f,	-0.5f, 0}, {0,0,0},
					{ 0.5f,	-0.5f, 0}, {0,0,0},
					{-0.5f,	 0.5f, 0}, {0,0,0},
					{ 0.5f,	 0.5f, 0}, {0,0,0}
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
					{ "NORMAL", Engine::LType::Float, 3},
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
			entity.AddComponent<Engine::MeshRendererComponent>(_standardLit);
			entity.GetTransform().position.x += 1.5;
		}

		/* Gltf Mesh */
		{
			/* Import Gltf Mesh */
			auto gltfMesh = std::make_shared<Engine::Mesh>("Test");
			//auto model = Engine::GltfIO::LoadFile("Resources/Models/Stanford_Dragon/FullRes.gltf");
			spdlog::stopwatch sw;
			auto model = Engine::GltfIO::LoadFile("Resources/Models/DamagedHelmet/gltf/DamagedHelmet.gltf");
			ENGINE_TRACE("Load Model: {}", sw);
			//auto model = Engine::GltfIO::LoadFile("Resources/Models/Sphere/Sphere.gltf");

			sw.reset();
			const auto& mesh = model.meshes[0];
			for (const auto& primitive : mesh.primitives) {
				auto vertexArray = Engine::GltfIO::LoadPrimitive(model, primitive);
				gltfMesh->AddSubmesh(vertexArray);
			}
			ENGINE_TRACE("Load Primitives: {}", sw);

			auto s = model.nodes[0].scale;
			glm::vec3 scale = s.size() > 0 ? glm::vec3(s[0], s[1], s[2]) : glm::vec3(1.0f);

			testEntity = _scene->CreateEntity("GLTF Mesh");
			auto& t = testEntity.GetTransform();
			t.scale = scale;
			t.rotation.x += 90;

			testEntity.AddComponent<Engine::MeshFilterComponent>(gltfMesh);
			testEntity.AddComponent<Engine::MeshRendererComponent>(_standardLit);
		}

		/* Camera */
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
			{
				spdlog::stopwatch sw;
				std::string path = "Resources/Skyboxes/kloofendal_48d_partly_cloudy_puresky_4k.hdr";
				auto hdrTexture2D = Engine::Texture2D::Utils::FromFile(path);
				ENGINE_TRACE("HDR Load: {}", sw);

				sw.reset();
				cc.skyboxCubemap = Engine::TextureCubeMap::Utils::FromTexture2D(hdrTexture2D, Engine::TextureCubeMap::Utils::Texture2DCubemapFormat::Equirectangle);
				ENGINE_TRACE("Cubemap Load: {}", sw);
			}
		}

		_fpsCameraController = FPSCameraController(_inputManager);
		_orbitCameraController = OrbitCameraController(_inputManager);
		_window->Subscribe<Engine::WindowMouseScrolledEvent>([&](const Engine::WindowMouseScrolledEvent& e) {
			_orbitCameraController.OnScroll((float)e.yOffset);
															 });
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

		_standardLit->SetUniform("cameraPosition", _camera.GetTransform().position);

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
