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

#include "amqp_client/rabbitmq_c_api.hpp"
#include "amqp_client/amqp_error.hpp"

class TestAMQPError : public ::testing::Test
{
};

TEST_F(TestAMQPError, empty_constructor)
{
  using amqp_client::AMQPError;
  using amqp_client::AMQPExceptionType;
  AMQPError error;
  EXPECT_FALSE(error);
  EXPECT_EQ(error.get_type(), AMQPExceptionType::NORMAL);
  EXPECT_EQ(error.get_server_error(), nullptr);
  EXPECT_EQ(error.get_library_error(), "operation completed successfully");
  EXPECT_EQ(error.str(), "operation completed successfully");
}

TEST_F(TestAMQPError, no_error)
{
  using amqp_client::AMQPError;
  using amqp_client::AMQPExceptionType;

  // Normal AMQP response
  amqp_rpc_reply_t reply;
  reply.reply_type = AMQP_RESPONSE_NORMAL;

  AMQPError error;
  bool result = AMQPError::from_amqp_rpc_reply(&reply, &error);
  EXPECT_FALSE(result);

  EXPECT_EQ(error.get_type(), AMQPExceptionType::NORMAL);
  EXPECT_EQ(error.str(), "operation completed successfully");
}

TEST_F(TestAMQPError, none_error)
{
  using amqp_client::AMQPError;
  using amqp_client::AMQPExceptionType;

  // Missing RPC type
  amqp_rpc_reply_t reply;
  reply.reply_type = AMQP_RESPONSE_NONE;

  AMQPError error;
  bool result = AMQPError::from_amqp_rpc_reply(&reply, &error);
  EXPECT_TRUE(result);

  EXPECT_EQ(error.get_type(), AMQPExceptionType::NONE);
  EXPECT_EQ(error.str(), "missing RPC reply type!");
}

TEST_F(TestAMQPError, library_error)
{
  using amqp_client::AMQPError;
  using amqp_client::AMQPExceptionType;

  {
    // host name look up
    amqp_rpc_reply_t reply;
    reply.reply_type = AMQP_RESPONSE_LIBRARY_EXCEPTION;
    reply.library_error = AMQP_STATUS_HOSTNAME_RESOLUTION_FAILED;

    AMQPError error;
    bool result = AMQPError::from_amqp_rpc_reply(&reply, &error);
    EXPECT_TRUE(result);

    EXPECT_EQ(error.get_type(), AMQPExceptionType::LIBRARY);
    EXPECT_EQ(error.str(), "hostname lookup failed");
  }

  {
    // bad AMQP data
    amqp_rpc_reply_t reply;
    reply.reply_type = AMQP_RESPONSE_LIBRARY_EXCEPTION;
    reply.library_error = AMQP_STATUS_BAD_AMQP_DATA;

    AMQPError error;
    bool result = AMQPError::from_amqp_rpc_reply(&reply, &error);
    EXPECT_TRUE(result);

    EXPECT_EQ(error.get_type(), AMQPExceptionType::LIBRARY);
    EXPECT_EQ(error.str(), "invalid AMQP data");
  }
}


TEST_F(TestAMQPError, server_error)
{
  using amqp_client::AMQPError;
  using amqp_client::AMQPExceptionType;

  {
    // connection close
    amqp_rpc_reply_t reply;
    reply.reply_type = AMQP_RESPONSE_SERVER_EXCEPTION;
    reply.reply.id = AMQP_CONNECTION_CLOSE_METHOD;
    amqp_connection_close_t reply_data;
    reply.reply.decoded = &reply_data;
    reply_data.reply_code = 1;
    std::string reply_text = "random text";
    reply_data.reply_text.bytes = reply_text.data();
    reply_data.reply_text.len = reply_text.size();

    AMQPError error;
    bool result = AMQPError::from_amqp_rpc_reply(&reply, &error);
    EXPECT_TRUE(result);

    EXPECT_EQ(error.get_type(), AMQPExceptionType::SERVER);
    EXPECT_EQ(error.str(), "server connection error 1, message: random text");
  }

  {
    // channel close
    amqp_rpc_reply_t reply;
    reply.reply_type = AMQP_RESPONSE_SERVER_EXCEPTION;
    reply.reply.id = AMQP_CHANNEL_CLOSE_METHOD;
    amqp_channel_close_t reply_data;
    reply.reply.decoded = &reply_data;
    reply_data.reply_code = 1;
    std::string reply_text = "random text";
    reply_data.reply_text.bytes = reply_text.data();
    reply_data.reply_text.len = reply_text.size();

    AMQPError error;
    bool result = AMQPError::from_amqp_rpc_reply(&reply, &error);
    EXPECT_TRUE(result);

    EXPECT_EQ(error.get_type(), AMQPExceptionType::SERVER);
    EXPECT_EQ(error.str(), "server channel error 1, message: random text");
  }

  {
    // unknown error
    amqp_rpc_reply_t reply;
    reply.reply_type = AMQP_RESPONSE_SERVER_EXCEPTION;
    reply.reply.id = AMQP_EXCHANGE_DECLARE_METHOD;

    AMQPError error;
    bool result = AMQPError::from_amqp_rpc_reply(&reply, &error);
    EXPECT_TRUE(result);

    EXPECT_EQ(error.get_type(), AMQPExceptionType::SERVER);
    EXPECT_EQ(error.str(), "unknown server error, method id 0x0028000A");
  }
}
