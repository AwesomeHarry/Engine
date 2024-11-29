#pragma once
#include <cstdint>
#include <memory>

#include "BufferCommon.h"

#include "VertexBufferObject.h"
#include "IndexBufferObject.h"

namespace Engine {
	enum class DrawMode {
		Points = 0x0000,
		Lines = 0x0001,
		LineLoop = 0x0002,
		LineStrip = 0x0003,
		Triangles = 0x0004,
		TriangleStrip = 0x0005,
		TriangleFan = 0x0006
	};

	class VertexArrayObject {
	public:
		VertexArrayObject();
		~VertexArrayObject();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBufferObject>& vertexBuffer);
		void SetIndexBuffer(const std::shared_ptr<IndexBufferObject>& indexBuffer);

		void Compute();

		inline const IndexBufferObject& GetIndexBuffer() const { return *_indexBuffer; }
		inline uint32_t GetVertexBufferCount() const { return (uint32_t)_vertexBuffers.size(); }
		inline const VertexBufferObject& GetVertexBuffer(int index) const { return *_vertexBuffers[index]; }

		inline bool HasIndices() const { return _hasIndices; }
		inline uint32_t GetCount() const { return _hasIndices ? _indexBuffer->GetCount() : _vertexBuffers[0]->GetCount(); }

		inline DrawMode GetDrawMode() const { return _drawMode; }
		inline void SetDrawMode(DrawMode mode) { _drawMode = mode; }

		bool show = true;
	private:
		uint32_t _id;
		std::vector<std::shared_ptr<VertexBufferObject>> _vertexBuffers;

		bool _hasIndices;
		std::shared_ptr<IndexBufferObject> _indexBuffer;
		DrawMode _drawMode = DrawMode::Triangles;
	};
}
