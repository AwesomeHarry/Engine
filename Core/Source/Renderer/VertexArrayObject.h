#pragma once
#include <cstdint>
#include <memory>

#include "Buffer.h"

#include "VertexBufferObject.h"
#include "IndexBufferObject.h"

namespace Engine {
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
		inline int GetVertexBufferCount() const { return _vertexBuffers.size(); }
		inline const VertexBufferObject& GetVertexBuffer(int index) const { return *_vertexBuffers[index]; }

		inline bool HasIndices() const { return _hasIndices; }
		inline uint64_t GetCount() const { return _hasIndices ? _indexBuffer->GetCount() : _vertexBuffers[0]->GetCount(); }
	private:
		uint32_t _id;
		std::vector<std::shared_ptr<VertexBufferObject>> _vertexBuffers;

		bool _hasIndices;
		std::shared_ptr<IndexBufferObject> _indexBuffer;
	};
}
