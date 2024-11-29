#pragma once

namespace Engine {
	class Entity;

	struct BaseComponent {
		virtual void OnComponentAdded(Entity& entity) {};
	};
}