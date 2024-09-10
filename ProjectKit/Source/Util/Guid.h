#pragma once

#include <random>
#include <string>

namespace Engine {
	class GUID {
	public:
		// Generates random GUID
		GUID() {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_int_distribution<> dis(0, 15);

			const char* chars = "0123456789abcdef";
			for (int i = 0; i < 32; i++) {
				_guid += chars[dis(gen)];
			}
		}
		GUID(const std::string& guid) : _guid(guid) {}

		std::string ToString() const { return _guid; }

		static GUID Invalid() { return GUID(""); }

	private:
		std::string _guid;
	};
}