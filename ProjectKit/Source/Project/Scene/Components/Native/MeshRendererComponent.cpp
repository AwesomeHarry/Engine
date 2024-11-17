#include "MeshRendererComponent.h"

#include "Logging/Logging.h"
#include "Project/Scene/Entity.h"
#include "MeshFilterComponent.h"

using namespace Engine;

void MeshRendererComponent::OnComponentAdded(Entity& entity) {
	if (!entity.HasComponent<MeshFilterComponent>())
		ENGINE_TRACE("[MeshRendererComponent::OnComponentAdded] Entity '{}' auto added MeshFilter", entity.GetName())
}

void MeshRendererComponent::SetUniform(const std::string& name, const UniformValue& value) {
	if (!materialAsset->UniformExists(name)) {
		ENGINE_WARN("Cannot override uniform '{}', it does not exist in the material", name);
		return;
	}

	_uniformOverrides[name] = value;
}

void MeshRendererComponent::Bind() const {
	auto material = materialAsset->GetInternal();

	for (const auto& [name, value] : _uniformOverrides) {
		material->SetUniform(name, value);
	}

	material->Bind();
}

void MeshRendererComponent::Unbind() const {
	materialAsset->GetInternal()->Unbind();
}