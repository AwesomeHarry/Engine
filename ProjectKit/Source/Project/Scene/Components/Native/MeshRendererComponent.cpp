#include "MeshRendererComponent.h"

#include "Logging/Logging.h"
#include "Project/Scene/Entity.h"
#include "MeshFilterComponent.h"

using namespace Engine;

void MeshRendererComponent::OnComponentAdded(Entity& entity) {
	if (!entity.HasComponent<MeshFilterComponent>())
		ENGINE_TRACE("[MeshRendererComponent::OnComponentAdded] Entity '{}' auto added MeshFilter", entity.GetName())
}