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

#ifndef AMQP_CLIENT__CHANNEL_RABBITMQ_C_HPP_
#define AMQP_CLIENT__CHANNEL_RABBITMQ_C_HPP_

#include <memory>
#include <string>

#include "amqp_client/macros.hpp"
#include "amqp_client/channel.hpp"
#include "amqp_client/rabbitmq_c_api.hpp"
#include "amqp_client/amqp_error.hpp"

namespace amqp_client
{

namespace rabbitmq_c
{

class Channel : public amqp_client::Channel
{
public:
  Channel(
    amqp_connection_state_t connection,
    amqp_channel_t channel_id,
    const std::shared_ptr<RabbitmqCInterface> & rabbitmq_c_interface
  );

  virtual ~Channel();

  bool exchange_declare(
    const std::string & exchange,
    const std::string & exchange_type = "direct",
    bool passive = false,
    bool durable = false,
    bool auto_delete = false,
    bool internal = false,
    AMQPError * error = nullptr
  ) override;

  std::string queue_declare(
    const std::string & queue,
    bool passive = false,
    bool durable = false,
    bool exclusive = false,
    bool auto_delete = false,
    AMQPError * error = nullptr
  ) override;

  bool queue_bind(
    const std::string & queue,
    const std::string & exchange,
    const std::string & routing_key,
    AMQPError * error = nullptr
  ) override;

  bool basic_publish(
    const std::string & exchange,
    const std::string & routing_key,
    const std::string & body,
    bool mandatory = false,
    bool immediate = false,
    AMQPError * error = nullptr
  ) override;

  std::string basic_consume(
    const std::string & queue,
    MessageCallback on_message_callback,
    bool no_local = false,
    bool auto_ack = false,
    bool exclusive = false,
    const std::string & consumer_tag = "",
    AMQPError * error = nullptr
  ) override;

  void start_consuming(double timeout_sec = -1) override;

  bool close(
    AMQPError * error = nullptr
  ) override;

private:
  AMQP_CLIENT_DISABLE_COPY(Channel)

  amqp_connection_state_t connection_;
  amqp_channel_t channel_id_;
  std::shared_ptr<RabbitmqCInterface> rabbitmq_c_interface_;
  MessageCallback on_message_callback_;
};

}  // namespace rabbitmq_c
}  // namespace amqp_client

#endif  // AMQP_CLIENT__CHANNEL_RABBITMQ_C_HPP_
