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

#ifndef AMQP_CLIENT__DEFAULT_LOG_HANDLER_HPP_
#define AMQP_CLIENT__DEFAULT_LOG_HANDLER_HPP_

#include <memory>

#include "amqp_client/log.hpp"

namespace spdlog
{
class logger;
}  // namespace spdlog

namespace amqp_client
{

namespace log
{
/// LogHandler object for default handling of logging messages.
/**
 * This class is used when no other LogHandler is registered
 */
class DefaultLogHandler : public LogHandler
{
public:
  /// Construct a new DefaultLogHandler object
  DefaultLogHandler();

  /// Construct a new DefaultLogHandler with a custom spdlog object
  explicit DefaultLogHandler(std::shared_ptr<spdlog::logger> logger);

  /// Function to log a message
  /**
   * \param[in] file The log message comes from this file
   * \param[in] line The log message comes from this line
   * \param[in] loglevel Indicates the severity of the log message
   * \param[in] log Log message
   */
  void log(const char * file, int line, LogLevel loglevel, const char * log) override;
};

}  // namespace log

}  // namespace amqp_client

#endif  // AMQP_CLIENT__DEFAULT_LOG_HANDLER_HPP_
