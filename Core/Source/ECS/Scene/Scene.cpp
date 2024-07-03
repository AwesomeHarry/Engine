#include "Scene.h"

#include "Entity.h"
#include "ECS/Components/Components.h"

using namespace Engine;

Entity Scene::CreateEntity(const std::string& name) {
	Entity entity(_registry.create(), this);

	// Add Name
	auto& nc = entity.AddComponent<NameComponent>(name);
	nc.name = name.empty() ? "Unamed Entity" : name;

	// Add Transform
	entity.AddComponent<TransformComponent>();

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