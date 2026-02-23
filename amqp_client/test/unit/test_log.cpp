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

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../utils/string_utils.hpp"
#include "amqp_client/log.hpp"

namespace amqp_client
{

namespace log
{

class MockLogHandler : public LogHandler
{
public:
  MockLogHandler() = default;
  virtual ~MockLogHandler()
  {
    die();
  }

  MOCK_METHOD(void, log, (const char *, int, LogLevel, const char *), (override));

  MOCK_METHOD(void, die, ());
};

}  // namespace log
}  // namespace amqp_client


class TestLog : public ::testing::Test
{
public:
  void log_all()
  {
    AMQP_CLIENT_LOG_DEBUG("hello from debug");
    AMQP_CLIENT_LOG_INFO("hello from info");
    AMQP_CLIENT_LOG_WARN("hello from warn");
    AMQP_CLIENT_LOG_ERROR("hello from error");
    AMQP_CLIENT_LOG_FATAL("hello from fatal");
  }

  void log_none()
  {
    // This is a potential edge case
    using amqp_client::log::log;
    using amqp_client::LogLevel;
    log(__FILE__, __LINE__, LogLevel::NONE, "hello from none");
  }

  void TearDown() override
  {
    using amqp_client::log::unregisterLogHandler;
    using amqp_client::log::setLogLevel;
    using amqp_client::LogLevel;

    unregisterLogHandler();
    setLogLevel(LogLevel::INFO);
  }
};

TEST_F(TestLog, log_default) {
  using amqp_client::log::getLogLevel;
  using amqp_client::LogLevel;

  // Default log level is INFO
  EXPECT_EQ(getLogLevel(), LogLevel::INFO);
  log_all();
  log_none();
}


TEST_F(TestLog, register_mock_LogHandler) {
  using amqp_client::log::log;
  using amqp_client::LogLevel;
  using amqp_client::log::MockLogHandler;
  using amqp_client::log::registerLogHandler;
  using ::testing::_;
  using ::testing::StrEq;

  auto mock_log_handler = std::make_unique<MockLogHandler>();
  MockLogHandler * mock_log_handler_raw_ptr = mock_log_handler.get();
  registerLogHandler(std::move(mock_log_handler));

  // No DEBUG logging call
  EXPECT_CALL(*mock_log_handler_raw_ptr, log(_, _, LogLevel::DEBUG, _)).Times(0);
  log(__FILE__, __LINE__, LogLevel::DEBUG, "hello from debug");

  // INFO
  EXPECT_CALL(
    *mock_log_handler_raw_ptr,
    log(__FILE__, __LINE__ + 2, LogLevel::INFO, StrEq("hello from info"))
  ).Times(1);
  log(__FILE__, __LINE__, LogLevel::INFO, "hello from info");

  // WARN
  EXPECT_CALL(
    *mock_log_handler_raw_ptr,
    log(__FILE__, __LINE__ + 2, LogLevel::WARN, StrEq("hello from warn"))
  ).Times(1);
  log(__FILE__, __LINE__, LogLevel::WARN, "hello from warn");

  // ERROR
  EXPECT_CALL(
    *mock_log_handler_raw_ptr,
    log(__FILE__, __LINE__ + 2, LogLevel::ERROR, StrEq("hello from error"))
  ).Times(1);
  log(__FILE__, __LINE__, LogLevel::ERROR, "hello from error");

  // FATAL
  EXPECT_CALL(
    *mock_log_handler_raw_ptr,
    log(__FILE__, __LINE__ + 2, LogLevel::FATAL, StrEq("hello from fatal"))
  ).Times(1);
  log(__FILE__, __LINE__, LogLevel::FATAL, "hello from fatal");

  // Cleanup is expected
  EXPECT_CALL(*mock_log_handler_raw_ptr, die()).Times(1);
}

TEST_F(TestLog, register_null_LogHandler) {
  using amqp_client::log::log;
  using amqp_client::LogLevel;
  using amqp_client::log::registerLogHandler;
  registerLogHandler(nullptr);

  log(__FILE__, __LINE__, LogLevel::INFO, "hello from default logger");
}

TEST_F(TestLog, unregister_mock_LogHandler) {
  using amqp_client::LogLevel;
  using amqp_client::log::log;
  using amqp_client::log::MockLogHandler;
  using amqp_client::log::registerLogHandler;
  using amqp_client::log::unregisterLogHandler;

  auto mock_log_handler = std::make_unique<MockLogHandler>();
  MockLogHandler * mock_log_handler_raw_ptr = mock_log_handler.get();
  registerLogHandler(std::move(mock_log_handler));

  // Check the mock handler is freed
  EXPECT_CALL(*mock_log_handler_raw_ptr, die()).Times(1);
  unregisterLogHandler();

  log(__FILE__, __LINE__, LogLevel::INFO, "hello from default logger");
}

TEST_F(TestLog, log_level) {
  using amqp_client::log::setLogLevel;
  using amqp_client::log::getLogLevel;
  using amqp_client::LogLevel;

  // Default log level is INFO
  EXPECT_EQ(getLogLevel(), LogLevel::INFO);

  // log level set to WARN
  setLogLevel(LogLevel::WARN);
  EXPECT_EQ(getLogLevel(), LogLevel::WARN);
  log_all();

  // log level set to DEBUG
  setLogLevel(LogLevel::DEBUG);
  EXPECT_EQ(getLogLevel(), LogLevel::DEBUG);
  log_all();

  // log level set to NONE
  setLogLevel(LogLevel::NONE);
  EXPECT_EQ(getLogLevel(), LogLevel::NONE);
  log_all();
}

TEST_F(TestLog, long_buffer) {
  using amqp_client::test_utils::generate_random_string;
  using amqp_client::log::MockLogHandler;
  using ::testing::_;
  using ::testing::StrEq;

  auto mock_log_handler = std::make_unique<MockLogHandler>();
  MockLogHandler * mock_log_handler_raw_ptr = mock_log_handler.get();
  registerLogHandler(std::move(mock_log_handler));

  std::string long_buffer = generate_random_string(2048);
  EXPECT_CALL(*mock_log_handler_raw_ptr, log(_, _, _, StrEq(long_buffer.c_str()))).Times(1);
  AMQP_CLIENT_LOG_INFO("%s", long_buffer.c_str());

  // Cleanup is expected
  EXPECT_CALL(*mock_log_handler_raw_ptr, die()).Times(1);
}
