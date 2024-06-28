#pragma once

#include <functional>
#include <map>
#include <type_traits>
#include "Event.h"


namespace Engine {
    template <typename EventType>
    class EventDispatcher {
    public:
        template <typename T>
        void Subscribe(const std::function<void(const T&)>& observer) {
            static_assert(std::is_base_of<Event, T>::value, "T must inherit from Event");
            std::string descriptor = T::GetStaticType();
            _observers[descriptor].push_back([observer](const Event& event) {
                observer(static_cast<const T&>(event));
            });
        }
    protected:
        void Dispatch(const EventType& event) {
            std::string descriptor = event.Type();
            if (_observers.find(descriptor) != _observers.end()) {
                for (const auto& observer : _observers[descriptor]) {
                    observer(event);
                }
            }
        }
    private:
        std::map<std::string, std::vector<std::function<void(const Event&)>>> _observers;
    };
}