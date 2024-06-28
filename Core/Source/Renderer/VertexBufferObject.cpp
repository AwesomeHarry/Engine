#include "VertexBufferObject.h"

#include <glad/glad.h>

using namespace Engine;

VertexBufferObject::VertexBufferObject(BufferUsage usage) : _id(0), _usage(usage), _count(0) {
	glGenBuffers(1, &_id);
	glBindBuffer(GL_ARRAY_BUFFER, _id);
}

VertexBufferObject::~VertexBufferObject() {
	glDeleteBuffers(1, &_id);
}

void VertexBufferObject::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VertexBufferObject::Unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::SetData(const void* data, uint64_t count, const VertexLayout& layout) {
	_layout = layout; _count = count;
	glBindBuffer(GL_ARRAY_BUFFER, _id);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(count * layout.GetStride()), data, (GLenum)_usage);
}
