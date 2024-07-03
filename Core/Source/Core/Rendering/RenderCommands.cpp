#include "RenderCommands.h"

#include <glad/glad.h>

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"

using namespace Engine;

void RenderCommands::SetClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void RenderCommands::ClearBuffers(const std::initializer_list<BufferBit>& bufferBits) {
	uint32_t clearBitmask = 0;
	for (auto& bit : bufferBits)
		clearBitmask |= (uint32_t)bit;
	glClear(clearBitmask);
}

void RenderCommands::SetWireframe(WireframeMode wireframeMode) {
	glPolygonMode(GL_FRONT_AND_BACK, (uint32_t)wireframeMode);
}

void RenderCommands::RenderMesh(const Mesh& mesh, const Shader& shader) {
	shader.Bind();

	for (size_t i = 0; i < mesh.GetSubmeshCount(); i++) {
		const auto& vertexArray = mesh.GetSubmesh(i);

		vertexArray.Bind();

		// TODO: Change to submit to renderer
		if (vertexArray.HasIndices())
			glDrawElements(GL_TRIANGLES, vertexArray.GetCount(), (GLenum)vertexArray.GetIndexBuffer().GetType(), 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, vertexArray.GetCount());
	}

	shader.Unbind();
}