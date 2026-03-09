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

#ifndef AMQP_CLIENT__CHANNEL_HPP_
#define AMQP_CLIENT__CHANNEL_HPP_

#include <functional>
#include <memory>
#include <string>

#include "amqp_client/macros.hpp"
#include "amqp_client/amqp_error.hpp"
#include "amqp_client/envelope.hpp"

namespace amqp_client
{

class Channel {
public:
  using MessageCallback = std::function<void(const Envelope &)>;

  virtual ~Channel() {}

  virtual bool exchange_declare(
    const std::string & exchange,
    const std::string & exchange_type = "direct",
    bool passive=false,
    bool durable=false,
    bool auto_delete=false,
    bool internal=false,
    AMQPError * error = nullptr
  ) = 0;

  virtual std::string queue_declare(
    const std::string & queue,
    bool passive=false,
    bool durable=false,
    bool exclusive=false,
    bool auto_delete=false,
    AMQPError * error = nullptr
  ) = 0;

  virtual bool queue_bind(
    const std::string & queue,
    const std::string & exchange,
    const std::string & routing_key,
    AMQPError * error = nullptr
  ) = 0;

  virtual bool basic_publish(
    const std::string & exchange,
    const std::string & routing_key,
    const std::string & body,
    bool mandatory = false,
    bool immediate = false,
    AMQPError * error = nullptr
  ) = 0;

  virtual std::string basic_consume(
    const std::string & queue,
    MessageCallback on_message_callback,
    bool no_local=false,
    bool auto_ack=false,
    bool exclusive=false,
    const std::string & consumer_tag="",
    AMQPError * error = nullptr
  ) = 0;

  virtual void start_consuming(double timeout_sec = -1) = 0;

  virtual bool close(
    AMQPError * error = nullptr
  ) = 0;

private:
  AMQP_CLIENT_DISABLE_COPY(Channel)
};

}  // namespace amqp_client

#endif  // AMQP_CLIENT__CHANNEL_HPP_
