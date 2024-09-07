#pragma once

#include "BaseComponent.h"
#include <string>

namespace Engine {
	class Entity;

	struct NameComponent : public BaseComponent {
		std::string name;

		NameComponent() {}
		NameComponent(const std::string& name)
			: name(name) {}
	};
}