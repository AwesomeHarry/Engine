#include "RenderCommands.h"

#include <glad/glad.h>

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
