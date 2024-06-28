#include "VertexArrayObject.h"

#include <glad/glad.h>

using namespace Engine;

VertexArrayObject::VertexArrayObject() : _id(0), _hasIndices(false) {
	glGenVertexArrays(1, &_id);
	glBindVertexArray(_id);
}

VertexArrayObject::~VertexArrayObject() {
	glDeleteVertexArrays(1, &_id);
}

void VertexArrayObject::Bind() const {
	glBindVertexArray(_id);
}

void VertexArrayObject::Unbind() const {
	glBindVertexArray(0);
}

void VertexArrayObject::AddVertexBuffer(const std::shared_ptr<VertexBufferObject>& vertexBuffer) {
	_vertexBuffers.push_back(vertexBuffer);
}

void VertexArrayObject::SetIndexBuffer(const std::shared_ptr<IndexBufferObject>& indexBuffer) {
	_hasIndices = true;
	_indexBuffer = indexBuffer;
}

void VertexArrayObject::Compute() {
	glBindVertexArray(_id);
	uint32_t index = 0;
	for (const auto& vertexBuffer : _vertexBuffers) {
		vertexBuffer->Bind();
		const auto& layout = vertexBuffer->GetLayout();
		uint32_t offset = 0;
		for (const auto& component : layout.GetComponents()) {
			glVertexAttribPointer(index, component.Count, (GLenum)component.Type, false, layout.GetStride(), (const void*)offset);
			glEnableVertexAttribArray(index);
			offset += component.GetByteSize();
			index++;
		}
	}
	_indexBuffer->Bind();
}