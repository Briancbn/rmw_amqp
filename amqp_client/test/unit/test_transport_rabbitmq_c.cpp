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

#include "amqp_client/rabbitmq_c_api.hpp"
#include "amqp_client/transport_rabbitmq_c.hpp"
#include "amqp_client/connection_info.hpp"
#include "amqp_client/amqp_error.hpp"
#include "amqp_client/transport.hpp"

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

static amqp_rpc_reply_t make_reply(amqp_response_type_enum type)
{
  amqp_rpc_reply_t reply{};
  reply.reply_type = type;
  return reply;
}

class TestTransportRabbitmqC : public ::testing::Test
{
public:
  void SetUp() override
  {
    mock_ = std::make_shared<amqp_client::MockRabbitmqCInterface>();
    transport_ = std::make_shared<amqp_client::rabbitmq_c::Transport>(mock_);
    info_ = std::make_shared<amqp_client::ConnectionInfo>();
  }

  std::shared_ptr<amqp_client::MockRabbitmqCInterface> mock_;
  std::shared_ptr<amqp_client::rabbitmq_c::Transport> transport_;
  std::shared_ptr<amqp_client::ConnectionInfo> info_;
};

TEST_F(TestTransportRabbitmqC, create_connection__new_connection_fails) {
  using ::testing::Return;

  EXPECT_CALL(*mock_, new_connection()).WillOnce(Return(nullptr));

  amqp_client::AMQPError err;
  auto conn = transport_->create_connection(info_, &err);

  EXPECT_EQ(conn, nullptr);
  EXPECT_TRUE(err);
  EXPECT_EQ(err.get_type(), amqp_client::AMQPExceptionType::CUSTOM);
}

TEST_F(TestTransportRabbitmqC, create_connection__socket_new_fails) {
  using ::testing::Return;
  using ::testing::_;

  amqp_connection_state_t fake_state = amqp_new_connection();
  EXPECT_CALL(*mock_, new_connection()).WillOnce(Return(fake_state));
  EXPECT_CALL(*mock_, tcp_socket_new(fake_state)).WillOnce(Return(nullptr));

  amqp_client::AMQPError err;
  auto conn = transport_->create_connection(info_, &err);

  EXPECT_EQ(conn, nullptr);
  EXPECT_TRUE(err);
  EXPECT_EQ(err.get_type(), amqp_client::AMQPExceptionType::CUSTOM);
}

TEST_F(TestTransportRabbitmqC, create_connection__socket_open_fails) {
  using ::testing::Return;
  using ::testing::_;

  amqp_connection_state_t fake_state = amqp_new_connection();
  auto fake_socket = reinterpret_cast<amqp_socket_t *>(fake_state);

  EXPECT_CALL(*mock_, new_connection()).WillOnce(Return(fake_state));
  EXPECT_CALL(*mock_, tcp_socket_new(fake_state)).WillOnce(Return(fake_socket));
  EXPECT_CALL(*mock_, socket_open(fake_socket, _, _))
  .WillOnce(Return(AMQP_STATUS_HOSTNAME_RESOLUTION_FAILED));

  amqp_client::AMQPError err;
  auto conn = transport_->create_connection(info_, &err);

  EXPECT_EQ(conn, nullptr);
  EXPECT_TRUE(err);
  EXPECT_EQ(err.get_type(), amqp_client::AMQPExceptionType::CUSTOM);
}

TEST_F(TestTransportRabbitmqC, create_connection__login_fails) {
  using ::testing::Return;
  using ::testing::_;

  amqp_connection_state_t fake_state = amqp_new_connection();
  auto fake_socket = reinterpret_cast<amqp_socket_t *>(fake_state);

  EXPECT_CALL(*mock_, new_connection()).WillOnce(Return(fake_state));
  EXPECT_CALL(*mock_, tcp_socket_new(fake_state)).WillOnce(Return(fake_socket));
  EXPECT_CALL(*mock_, socket_open(_, _, _)).WillOnce(Return(AMQP_STATUS_OK));
  EXPECT_CALL(*mock_, login(_, _, _, _, _, _))
  .WillOnce(Return(make_reply(AMQP_RESPONSE_LIBRARY_EXCEPTION)));

  amqp_client::AMQPError err;
  auto conn = transport_->create_connection(info_, &err);

  EXPECT_EQ(conn, nullptr);
  EXPECT_TRUE(err);
}

TEST_F(TestTransportRabbitmqC, create_connection__success) {
  using ::testing::Return;
  using ::testing::_;

  amqp_connection_state_t fake_state = amqp_new_connection();
  auto fake_socket = reinterpret_cast<amqp_socket_t *>(fake_state);

  EXPECT_CALL(*mock_, new_connection()).WillOnce(Return(fake_state));
  EXPECT_CALL(*mock_, tcp_socket_new(fake_state)).WillOnce(Return(fake_socket));
  EXPECT_CALL(*mock_, socket_open(_, _, _)).WillOnce(Return(AMQP_STATUS_OK));
  EXPECT_CALL(*mock_, login(_, _, _, _, _, _))
  .WillOnce(Return(make_reply(AMQP_RESPONSE_NORMAL)));

  amqp_client::AMQPError err;
  auto conn = transport_->create_connection(info_, &err);

  EXPECT_NE(conn, nullptr);
  EXPECT_FALSE(err);
}

TEST_F(TestTransportRabbitmqC, create_connection__null_error_does_not_crash) {
  using ::testing::Return;

  EXPECT_CALL(*mock_, new_connection()).WillOnce(Return(nullptr));

  auto conn = transport_->create_connection(info_, nullptr);
  EXPECT_EQ(conn, nullptr);
}

TEST_F(TestTransportRabbitmqC, create_default__returns_non_null) {
  auto transport = amqp_client::Transport::create_default();
  EXPECT_NE(transport, nullptr);
}
