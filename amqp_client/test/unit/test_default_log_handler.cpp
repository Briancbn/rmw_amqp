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


#include "gtest/gtest.h"
#include "amqp_client/log.hpp"

class TestLog : public ::testing::Test
{
};

TEST_F(TestLog, log) {
  using amqp_client::log::setLogLevel;
  using amqp_client::log::log;
  using amqp_client::LogLevel;

  setLogLevel(LogLevel::DEBUG);

  log(__FILE__, __LINE__, LogLevel::DEBUG, "hello from debug");
  log(__FILE__, __LINE__, LogLevel::INFO, "hello from info");
  log(__FILE__, __LINE__, LogLevel::WARN, "hello from warn");
  log(__FILE__, __LINE__, LogLevel::ERROR, "hello from error");
  log(__FILE__, __LINE__, LogLevel::FATAL, "hello from fatal");
}
