#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Renderer/VertexArrayObject.h"

namespace Engine {
	class Mesh {
	public:
		Mesh() {}
		Mesh(const std::string& name)
			: _name(name) {}
		~Mesh() {}

		void AddSubmesh(std::shared_ptr<VertexArrayObject> submesh);

		inline void SetName(const std::string& newName) { _name = newName; }
		inline const std::vector<std::shared_ptr<VertexArrayObject>>& GetSubmeshes() const { return _submeshes; }
		inline const VertexArrayObject& GetSubmesh(int index) const { return *_submeshes[index]; }
	private:
		std::string _name;
		std::vector<std::shared_ptr<VertexArrayObject>> _submeshes;
	};
}