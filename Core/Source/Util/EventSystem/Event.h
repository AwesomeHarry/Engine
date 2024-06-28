#pragma once
#include <string>

namespace Engine {
	class Event {
	public:
		virtual ~Event() = default;
		virtual std::string Type() const = 0;  // Ensure all events provide a unique type identifier
	};

#define EVENT_CLASS_TYPE(type) \
	public: \
	static std::string GetStaticType() { return #type; }\
    std::string Type() const override { return GetStaticType(); }
}