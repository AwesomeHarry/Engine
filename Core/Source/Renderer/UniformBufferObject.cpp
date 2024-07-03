#include "UniformBufferObject.h"
#include <glad/glad.h>

using namespace Engine;

UniformBufferObject::UniformBufferObject(uint32_t size, uint32_t bindingPoint) 
	: _bindingPoint(bindingPoint) {
	glGenBuffers(1, &_id);
	glBindBuffer(GL_UNIFORM_BUFFER, _id);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, _id);
}

UniformBufferObject::~UniformBufferObject() {
	glDeleteBuffers(1, &_id);
}

void UniformBufferObject::Bind() {
	glBindBuffer(GL_UNIFORM_BUFFER, _id);
}

void UniformBufferObject::Unbind() {
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBufferObject::SetData(const void* data, uint32_t size, uint32_t offset) {
	glBindBuffer(GL_UNIFORM_BUFFER, _id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
