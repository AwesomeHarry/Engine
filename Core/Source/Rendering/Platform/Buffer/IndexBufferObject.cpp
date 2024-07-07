#include "IndexBufferObject.h"

#include <glad/glad.h>

using namespace Engine;

IndexBufferObject::IndexBufferObject(BufferUsage usage) : _id(0), _usage(usage) {
	glGenBuffers(1, &_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

IndexBufferObject::~IndexBufferObject() {
	glDeleteBuffers(1, &_id);
}

void IndexBufferObject::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

void IndexBufferObject::Unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBufferObject::SetData(const void* data, LType type, uint64_t count) {
	_type = type; _count = count;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(GetLTypeSize(type) * count), data, (GLenum)_usage);
}