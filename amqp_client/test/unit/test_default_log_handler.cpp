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
#include <sstream>

#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"
#include "amqp_client/default_log_handler.hpp"

class TestDefaultLogHandler : public ::testing::Test
{
public:
  void TearDown() override
  {
    using amqp_client::log::DefaultLogHandler;
    DefaultLogHandler::set_spdlog_default_logger(spdlog::default_logger());
  }
};

TEST_F(TestDefaultLogHandler, custom_default_spdlog_logger__oss_sink) {
  using amqp_client::log::DefaultLogHandler;
  using amqp_client::log::log;
  using amqp_client::LogLevel;

  std::ostringstream oss;
  auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_st>(oss);
  auto ostream_logger = std::make_shared<spdlog::logger>("ostream_logger", ostream_sink);
  DefaultLogHandler::set_spdlog_default_logger(ostream_logger);

  log(__FILE__, __LINE__, LogLevel::DEBUG, "hello from debug");
  EXPECT_TRUE(oss.str().empty());

  log(__FILE__, __LINE__, LogLevel::INFO, "hello from info");
  EXPECT_EQ(oss.str().substr(30), "INFO: hello from info (test_default_log_handler.cpp:46)\n");
}

TEST_F(TestDefaultLogHandler, custom_default_spdlog_logger__nullptr) {
  using amqp_client::log::DefaultLogHandler;
  using amqp_client::log::log;
  using amqp_client::LogLevel;

  DefaultLogHandler::set_spdlog_default_logger(nullptr);
  log(__FILE__, __LINE__, LogLevel::INFO, "hello from default logger");
}
