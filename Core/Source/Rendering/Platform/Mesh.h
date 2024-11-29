#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Rendering/Platform/Buffer/VertexArrayObject.h"

namespace Engine {
	class Mesh {
	public:
		Mesh() {}
		Mesh(const std::string& name)
			: _name(name) {}
		~Mesh() {}

		void AddSubmesh(std::shared_ptr<VertexArrayObject> submesh);

		inline void SetName(const std::string& newName) { _name = newName; }
		inline std::string GetName() const { return _name; }

		inline uint32_t GetSubmeshCount() const { return (uint32_t)_submeshes.size(); }
		inline const VertexArrayObject& GetSubmesh(int index) const { return *_submeshes[index]; }
	private:
		std::string _name;
		std::vector<std::shared_ptr<VertexArrayObject>> _submeshes;
	};
}