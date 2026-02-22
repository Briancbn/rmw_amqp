// Copyright 2026 Chen Bainian
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define SPDLOG_LEVEL_NAMES { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "OFF" }

#include <memory>
#include <filesystem>
#include "spdlog/spdlog.h"
#include "amqp_client/default_log_handler.hpp"

namespace amqp_client
{

namespace log
{

DefaultLogHandler::DefaultLogHandler()
{
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^%8l%$: %v");
  spdlog::set_level(spdlog::level::debug);
}

DefaultLogHandler::DefaultLogHandler(std::shared_ptr<spdlog::logger> logger)
{
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^%8l%$: %v");
  spdlog::set_level(spdlog::level::debug);
}

void DefaultLogHandler::log(
  const char * file,
  int line,
  LogLevel loglevel,
  const char * log
)
{
  std::filesystem::path full_path = file;
  auto filename = full_path.filename();
  switch (loglevel) {
    case LogLevel::INFO:
      spdlog::info("{} ({}:{})", log, filename.c_str(), line);
      break;
    case LogLevel::DEBUG:
      spdlog::debug("{} ({}:{})", log, filename.c_str(), line);
      break;
    case LogLevel::WARN:
      spdlog::warn("{} ({}:{})", log, filename.c_str(), line);
      break;
    case LogLevel::ERROR:
      spdlog::error("{} ({}:{})", log, filename.c_str(), line);
      break;
    case LogLevel::FATAL:
      spdlog::critical("{} ({}:{})", log, filename.c_str(), line);
      break;
    default:
      break;
  }
}

}  // namespace log

}  // namespace amqp_client
