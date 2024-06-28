#include "LoggingManager.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/base_sink.h>

#include "Logging/Logging.h""

using namespace Engine;

bool LoggingManager::Initialize() {
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%Y-%m-%d | %H:%M:%S.%e] %v%$");

	std::vector<spdlog::sink_ptr> sinks{ consoleSink };
	auto logger = std::make_shared<spdlog::logger>(ENGINE_DEFAULT_LOGGER_NAME, consoleSink);
	logger->set_level(spdlog::level::trace);
	logger->flush_on(spdlog::level::trace);
	spdlog::register_logger(logger);

	return true;
}

void LoggingManager::Shutdown() {
	spdlog::shutdown();
}
