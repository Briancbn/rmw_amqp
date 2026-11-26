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

// Subscriber example: receives messages from a ROS2-style topic via AMQP.
//
// Convention:
//   exchange      — shared "ros2" exchange, type "topic"
//   routing key   — dotted ROS2 topic name (e.g. "rt.chatter")
//                   binding pattern "rt.*" matches all topics under "rt"

#include <memory>
#include <string>

#include "amqp_client/log.hpp"
#include "amqp_client/amqp_error.hpp"
#include "amqp_client/connection_info.hpp"
#include "amqp_client/transport.hpp"
#include "amqp_client/connection.hpp"
#include "amqp_client/channel.hpp"
#include "amqp_client/envelope.hpp"
#include "amqp_client/message.hpp"

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

  const std::string exchange = "ros2";
  const std::string routing_key = "rt.chatter";       // exact topic to subscribe to
  const std::string binding_pattern = "rt.*";         // wildcard: all /rt/* topics

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

  // Declare the same shared exchange (idempotent if publisher ran first)
  channel->exchange_declare(exchange, "topic", false, true, false, false, &err);
  if (!check(err, "exchange_declare")) {return 1;}

  // Exclusive auto-delete queue — one per subscriber instance
  std::string queue = channel->queue_declare("", false, false, true, true, &err);
  if (!check(err, "queue_declare") || queue.empty()) {return 1;}
  AMQP_CLIENT_LOG_INFO_STREAM("Subscriber queue: " << queue);

  // Bind with the wildcard pattern so this subscriber receives all rt.* topics
  channel->queue_bind(queue, exchange, binding_pattern, &err);
  if (!check(err, "queue_bind")) {return 1;}
  AMQP_CLIENT_LOG_INFO_STREAM(
    "Bound queue to exchange '" << exchange << "' with pattern '" << binding_pattern << "'");

  amqp_client::Channel::MessageCallback on_message =
    [&](const amqp_client::Envelope & env) {
      AMQP_CLIENT_LOG_INFO_STREAM(
        "[" << env.exchange << " / " << env.routing_key << "]"
          " delivery_tag=" << env.delivery_tag <<
          " redelivered=" << env.redelivered <<
          " body=" << env.message.body);
    };

  // basic_consume(queue, callback, no_local, auto_ack, exclusive, consumer_tag)
  std::string consumer_tag =
    channel->basic_consume(queue, on_message, false, true, false, "", &err);
  if (!check(err, "basic_consume")) {return 1;}
  AMQP_CLIENT_LOG_INFO_STREAM("Consuming as: " << consumer_tag);

  AMQP_CLIENT_LOG_INFO("Waiting for messages (10 s timeout) ...");
  channel->start_consuming(10.0);

  channel->close(&err);
  check(err, "channel close");

  connection->close(&err);
  check(err, "connection close");

  return 0;
}
