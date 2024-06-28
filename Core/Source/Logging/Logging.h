#pragma once

#define ENGINE_DEFAULT_LOGGER_NAME "EngineLogger"

#include <spdlog/spdlog.h>

#define ENGINE_BREAK __debugbreak();

#define ENGINE_TRACE(...) { if (spdlog::get(ENGINE_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(ENGINE_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__); } }
#define ENGINE_INFO(...) { if (spdlog::get(ENGINE_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(ENGINE_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__); } }
#define ENGINE_WARN(...) { if (spdlog::get(ENGINE_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(ENGINE_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__); } }
#define ENGINE_ERROR(...) { if (spdlog::get(ENGINE_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(ENGINE_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__); } }
#define ENGINE_FATAL(...) { if (spdlog::get(ENGINE_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(ENGINE_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__); } }

#define ENGINE_ASSERT(condition, msg) { if (!(condition)) { ENGINE_FATAL("ASSERT FAILED - {}\n\t{}\n\tin file: {}\n\ton line: {}", #condition, msg, __FILE__, __LINE__); ENGINE_BREAK } }