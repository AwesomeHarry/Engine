#include "RenderCommands.h"

#include <glad/glad.h>

#include "Rendering/Platform/Mesh.h"
#include "Rendering/Platform/Shader.h"
#include "Rendering/Platform/Material.h"

using namespace Engine;

void RenderCommands::SetClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void RenderCommands::ClearBuffers(const FlagSet<BufferBit>& bufferBits) {
	GLbitfield bits = 0;
	if (bufferBits[BufferBit::Color])
		bits |= GL_COLOR_BUFFER_BIT;
	if (bufferBits[BufferBit::Depth])
		bits |= GL_DEPTH_BUFFER_BIT;
	if (bufferBits[BufferBit::Stencil])
		bits |= GL_STENCIL_BUFFER_BIT;
	glClear(bits);
}

void RenderCommands::SetViewport(int x, int y, int width, int height) {
	glViewport(x, y, width, height);
}

void RenderCommands::SetWireframe(WireframeMode wireframeMode) {
	glPolygonMode(GL_FRONT_AND_BACK, (uint32_t)wireframeMode);
}

#include "Rendering/Platform/Texture2D.h"

void RenderCommands::RenderMesh(const Mesh& mesh, const Material& material) {
	material.Bind();
	for (size_t i = 0; i < mesh.GetSubmeshCount(); i++) {
		auto& vertexArray = mesh.GetSubmesh(i);

		vertexArray.Bind();
		if (vertexArray.HasIndices())
			glDrawElements((uint32_t)vertexArray.GetDrawMode(), vertexArray.GetCount(), (GLenum)vertexArray.GetIndexBuffer().GetType(), 0);
		else
			glDrawArrays((uint32_t)vertexArray.GetDrawMode(), 0, vertexArray.GetCount());
	}
	material.Unbind();
}

void RenderCommands::RenderMesh(const VertexArrayObject& vertexArray, const Shader& shader) {
	shader.Bind();
	vertexArray.Bind();

	if (vertexArray.HasIndices())
		glDrawElements((uint32_t)vertexArray.GetDrawMode(), vertexArray.GetCount(), (GLenum)vertexArray.GetIndexBuffer().GetType(), 0);
	else
		glDrawArrays((uint32_t)vertexArray.GetDrawMode(), 0, vertexArray.GetCount());

	vertexArray.Unbind();
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
