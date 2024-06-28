#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include "Logging/Logging.h"

namespace Engine {
	class FileIO {
    public:
        static std::string ReadFile(const std::string& filePath) {
            std::ifstream file(filePath);
            std::stringstream ss;

            if (file.is_open()) {
                ss << file.rdbuf();
                file.close();
            }
            else {
                ENGINE_ERROR("Error: Could not open shader file: {}", filePath);
            }

            return ss.str();
        }
	};
}