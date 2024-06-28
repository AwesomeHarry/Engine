#include "Scene.h"

#include "Entity.h"
#include "ECS/Components/Components.h"

using namespace Engine;

Entity Scene::CreateEntity(const std::string& name) {
	Entity entity(_registry.create(), this);

	auto& tc = entity.AddComponent<NameComponent>(name);
	tc.name = name.empty() ? "Unamed Entity" : name;

	return entity;
}

void Scene::DestroyEntity(Entity entity) {
	_registry.destroy(entity.GetID());
}

Entity Scene::GetEntity(const std::string& name) {
	auto view = _registry.view<NameComponent>();
	for (auto entityUid : view) {
		const NameComponent& tc = view.get<NameComponent>(entityUid);	
		if (tc.name == name)
			return Entity(entityUid, this);
	}
	return {};
}

void Scene::UpdateScene(float ts) {

}

void Scene::RenderScene() {
	// Mesh renderers
	{
		auto view = _registry.view<const MeshFilterComponent, MeshRendererComponent>();
		for (auto entity : view) {
			auto& filter = view.get<MeshFilterComponent>(entity);
			auto& renderer = view.get<MeshRendererComponent>(entity);

			renderer.RenderMesh(*filter.mesh);
		}
	}
}