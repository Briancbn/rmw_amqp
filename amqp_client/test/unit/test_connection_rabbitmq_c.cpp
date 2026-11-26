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

// Unit tests for rabbitmq_c::Connection.
//
// Tests that require a live broker (channel open/close, login) belong in
// integration tests.  Here we cover only the paths that do not need a real
// connection: null-state guards and channel-id allocation.

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "amqp_client/rabbitmq_c_api.hpp"
#include "amqp_client/connection_rabbitmq_c.hpp"
#include "amqp_client/amqp_error.hpp"

namespace amqp_client
{

class MockRabbitmqCInterface : public RabbitmqCInterface
{
public:
  MOCK_METHOD(amqp_connection_state_t, new_connection, (), (override));
  MOCK_METHOD(amqp_socket_t *, tcp_socket_new, (amqp_connection_state_t), (override));
  MOCK_METHOD(int, socket_open, (amqp_socket_t *, const char *, int), (override));
  MOCK_METHOD(
    amqp_rpc_reply_t, login,
    (amqp_connection_state_t, const char *, int, int, int, const SASLConfig *),
    (override));
};

}  // namespace amqp_client

class TestConnectionRabbitmqC : public ::testing::Test
{
public:
  void SetUp() override
  {
    mock_ = std::make_shared<amqp_client::MockRabbitmqCInterface>();
  }

  std::shared_ptr<amqp_client::MockRabbitmqCInterface> mock_;
};

// ---------------------------------------------------------------------------
// channel() — null connection guard
// ---------------------------------------------------------------------------

TEST_F(TestConnectionRabbitmqC, channel__null_connection_returns_nullptr) {
  amqp_client::rabbitmq_c::Connection conn(nullptr, mock_);

  amqp_client::AMQPError err;
  auto ch = conn.channel(nullptr, &err);

  EXPECT_EQ(ch, nullptr);
  EXPECT_TRUE(err);
  EXPECT_EQ(err.get_type(), amqp_client::AMQPExceptionType::CUSTOM);
}

TEST_F(TestConnectionRabbitmqC, channel__null_connection_null_error_does_not_crash) {
  amqp_client::rabbitmq_c::Connection conn(nullptr, mock_);
  auto ch = conn.channel(nullptr, nullptr);
  EXPECT_EQ(ch, nullptr);
}

// ---------------------------------------------------------------------------
// close() — null connection guard
// ---------------------------------------------------------------------------

TEST_F(TestConnectionRabbitmqC, close__null_connection_returns_false) {
  amqp_client::rabbitmq_c::Connection conn(nullptr, mock_);

  amqp_client::AMQPError err;
  bool result = conn.close(&err);

  EXPECT_FALSE(result);
  EXPECT_TRUE(err);
  EXPECT_EQ(err.get_type(), amqp_client::AMQPExceptionType::CUSTOM);
}

TEST_F(TestConnectionRabbitmqC, close__null_connection_null_error_does_not_crash) {
  amqp_client::rabbitmq_c::Connection conn(nullptr, mock_);
  EXPECT_FALSE(conn.close(nullptr));
}
