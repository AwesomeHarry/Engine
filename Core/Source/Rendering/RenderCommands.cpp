#include "RenderCommands.h"

#include <glad/glad.h>

#include "Rendering/Platform/Mesh.h"
#include "Rendering/Platform/Shader.h"

using namespace Engine;

void RenderCommands::SetClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void RenderCommands::ClearBuffers(const std::vector<BufferBit>& bufferBits) {
	uint32_t clearBitmask = 0;
	for (auto& bit : bufferBits)
		clearBitmask |= (uint32_t)bit;
	glClear(clearBitmask);
}

void RenderCommands::SetViewport(int x, int y, int width, int height) {
	glViewport(x, y, width, height);
}

void RenderCommands::SetWireframe(WireframeMode wireframeMode) {
	glPolygonMode(GL_FRONT_AND_BACK, (uint32_t)wireframeMode);
}

void RenderCommands::RenderMesh(const Mesh& mesh, const Shader& shader) {
	shader.Bind();
	for (size_t i = 0; i < mesh.GetSubmeshCount(); i++) {
		auto& vertexArray = mesh.GetSubmesh(i);

		vertexArray.Bind();

		if (vertexArray.HasIndices())
			glDrawElements((uint32_t)vertexArray.GetDrawMode(), vertexArray.GetCount(), (GLenum)vertexArray.GetIndexBuffer().GetType(), 0);
		else
			glDrawArrays((uint32_t)vertexArray.GetDrawMode(), 0, vertexArray.GetCount());
	}
	shader.Unbind();
}

// Likely usage for geometry shaders
void RenderCommands::RenderPoints(const VertexArrayObject& vertexArray, uint64_t count, const Shader& shader) {
	shader.Bind();
	vertexArray.Bind();
	glDrawArrays(GL_POINTS, 0, count);
	vertexArray.Unbind();
	shader.Unbind();
}
