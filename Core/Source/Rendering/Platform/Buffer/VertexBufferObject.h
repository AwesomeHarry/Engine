#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "BufferCommon.h"

namespace Engine {
	struct VertexComponent {
		std::string Name;
		LType Type;
		uint32_t Count;

		size_t GetByteSize() const {
			return GetLTypeSize(Type) * Count;
		}
	};

	class VertexLayout {
	public:
		VertexLayout() = default;
		VertexLayout(std::initializer_list<VertexComponent> components)
			: _components(components) {}

		void AddComponent(const VertexComponent& component) {
			_components.push_back(component);
		}

		size_t GetStride() const {
			size_t stride = 0;
			for (const auto& component : _components)
				stride += component.GetByteSize();
			return stride;
		}

		const std::vector<VertexComponent>& GetComponents() const {
			return _components;
		}
	private:
		std::vector<VertexComponent> _components;
	};

	class VertexBufferObject {
	public:
		static const uint64_t DEFAULT_INITIAL_CAPACITY = 1000 * sizeof(float) * 4;

		VertexBufferObject(BufferUsage usage = BufferUsage::Static, uint64_t initialCapacity = DEFAULT_INITIAL_CAPACITY);
		~VertexBufferObject();

		VertexBufferObject(const VertexBufferObject&) = delete;
		VertexBufferObject& operator=(const VertexBufferObject&) = delete;

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint64_t count, const VertexLayout& layout);
		void UpdateSubData(const void* data, uint64_t offset, uint64_t size);

		inline const VertexLayout& GetLayout() const { return _layout; }
		inline uint64_t GetCount() const { return _count; }
		inline uint64_t GetCapacity() const { return _capacity; }
	private:
		uint32_t _id;
		BufferUsage _usage;
		VertexLayout _layout;
		uint64_t _count;
		uint64_t _capacity;
	};
}