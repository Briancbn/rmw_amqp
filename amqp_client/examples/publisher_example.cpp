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

// Publisher example: publishes messages to a ROS2-style topic via AMQP.
//
// Convention:
//   exchange      — shared "ros2" exchange, type "topic"
//   routing key   — dotted ROS2 topic name (e.g. "rt.chatter")
//                   subscribers can bind with exact keys or wildcards (rt.*)

#include <memory>
#include <string>

#include "amqp_client/log.hpp"
#include "amqp_client/amqp_error.hpp"
#include "amqp_client/connection_info.hpp"
#include "amqp_client/transport.hpp"
#include "amqp_client/connection.hpp"
#include "amqp_client/channel.hpp"

static bool check(const amqp_client::AMQPError & err, const char * context)
{
  if (!err) {
    return true;
  }
  AMQP_CLIENT_LOG_ERROR_STREAM(context << ": " << err.str());
  return false;
}

int main()
{
  amqp_client::log::setLogLevel(amqp_client::LogLevel::INFO);

  const std::string exchange    = "ros2";
  const std::string routing_key = "rt.chatter";   // ROS2 topic /chatter

  auto info = std::make_shared<amqp_client::ConnectionInfo>();
  info->host("localhost")->port(5672)->vhost("/")->username("guest")->password("guest");

  auto transport = amqp_client::Transport::create_default();

  amqp_client::AMQPError err;
  auto connection = transport->create_connection(info, &err);
  if (!check(err, "create_connection") || !connection) {
    AMQP_CLIENT_LOG_FATAL("Could not connect — is RabbitMQ running on localhost:5672?");
    return 1;
  }

  auto channel = connection->channel(nullptr, &err);
  if (!check(err, "channel") || !channel) {return 1;}

  // Shared durable "topic" exchange — consumers bind with routing key patterns
  channel->exchange_declare(exchange, "topic", false, true, false, false, &err);
  if (!check(err, "exchange_declare")) {return 1;}

  const int msg_count = 5;
  for (int i = 0; i < msg_count; ++i) {
    std::string body = "Hello World " + std::to_string(i);
    channel->basic_publish(exchange, routing_key, body, false, false, &err);
    if (!check(err, "basic_publish")) {return 1;}
    AMQP_CLIENT_LOG_INFO_STREAM(
      "[" << exchange << " / " << routing_key << "] Publishing: " << body);
  }

  channel->close(&err);
  check(err, "channel close");

  connection->close(&err);
  check(err, "connection close");

  return 0;
}
