#include "Mesh.h"

using namespace Engine;

void Mesh::AddSubmesh(std::shared_ptr<VertexArrayObject> submesh) {
	_submeshes.push_back(submesh);
}
