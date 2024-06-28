#include "MeshFilterComponent.h"

#include "ECS/Scene/Entity.h"
#include "Logging/Logging.h"

using namespace Engine;

void MeshRendererComponent::OnComponentAdded(Entity& entity) {
	if (!entity.HasComponent<MeshFilterComponent>())
		ENGINE_TRACE("Entity [{}] auto added MeshFilter", entity.GetName())
}

#include <glad/glad.h>

void MeshRendererComponent::RenderMesh(const Mesh& mesh) {
	shader->Bind();

	for (const auto& vertexArray : mesh.GetSubmeshes()) {
		vertexArray->Bind();

		// TODO: Change to submit to renderer
		if (vertexArray->HasIndices())
			glDrawElements(GL_TRIANGLES, vertexArray->GetCount(), (GLenum)vertexArray->GetIndexBuffer().GetType(), 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetCount());
	}

	shader->Unbind();
}
