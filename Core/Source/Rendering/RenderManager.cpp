#include "RenderManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Logging/Logging.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "Core/Application/Window.h"

using namespace Engine;

RenderManager::RenderManager(const GraphicsContext& graphicsContext)
	: _graphicsContext(graphicsContext), _wireframeMode(WireframeMode::Fill) {}

bool RenderManager::Initialize(Window& window) {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		ENGINE_ERROR("Failed to initialize GLAD");
		return false;
	}

	setContext();

	// Init ImGui Context
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	window.Subscribe<Engine::WindowResizeEvent>([](const Engine::WindowResizeEvent& event) {
		glViewport(0, 0, event.width, event.height);
	});

	_renderPipeline->Initialize();

	return true;
}

void RenderManager::Shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void RenderManager::BeginFrame() {
	RenderCommands::SetWireframe(_wireframeMode);

	// Start ImGui Frame
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	}
}

void RenderManager::EndFrame() {
	RenderCommands::SetWireframe(WireframeMode::Fill);

	// End ImGui Frame
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}

void RenderManager::setContext() {
	if (_graphicsContext.EnableDepthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (_graphicsContext.EnableBlend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glBlendFunc((GLenum)_graphicsContext.SrcBlendFactor, (GLenum)_graphicsContext.DstBlendFactor);

	if (_graphicsContext.EnableCullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if (_graphicsContext.CullFront && _graphicsContext.CullBack)
		glCullFace(GL_FRONT_AND_BACK);
	else if (_graphicsContext.CullFront)
		glCullFace(GL_FRONT);
	else if (_graphicsContext.CullBack)
		glCullFace(GL_BACK);
}

void RenderManager::RenderScene(Scene& scene) {
	_renderPipeline->RenderScene(scene);
}
