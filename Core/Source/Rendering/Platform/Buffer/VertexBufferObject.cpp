#include "VertexBufferObject.h"
#include <glad/glad.h>

using namespace Engine;

VertexBufferObject::VertexBufferObject(BufferUsage usage, uint64_t initialCapacity)
    : _id(0), _usage(usage), _count(0), _capacity(initialCapacity) {
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, _capacity * sizeof(float), nullptr, static_cast<GLenum>(_usage));
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

void VertexBufferObject::SetData(const void* data, uint32_t count, const VertexLayout& layout) {
    _layout = layout;
    _count = count;
    uint64_t requiredSize = count * layout.GetStride();

    glBindBuffer(GL_ARRAY_BUFFER, _id);

    if (requiredSize > _capacity) {
        // If the new data exceeds the current capacity, reallocate the buffer
        _capacity = requiredSize;
        glBufferData(GL_ARRAY_BUFFER, _capacity, nullptr, (GLenum)_usage);
    }

    // Use glBufferSubData to update the buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, requiredSize, data);
}

void VertexBufferObject::UpdateSubData(const void* data, uint64_t offset, uint64_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

std::vector<uint8_t> Engine::VertexBufferObject::GetRawData() const {
    std::vector<uint8_t> rawData(_count * _layout.GetStride());
    Bind();
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, rawData.size(), rawData.data());
    return rawData;
}