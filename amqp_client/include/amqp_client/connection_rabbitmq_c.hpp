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

#ifndef RMF2_SCHEDULER__AMQP__CONNECTION_RABBITMQ_C_HPP_
#define RMF2_SCHEDULER__AMQP__CONNECTION_RABBITMQ_C_HPP_

#include <memory>
#include <string>
#include <unordered_set>

#include "amqp_client/macros.hpp"
#include "amqp_client/rabbitmq_c_api.hpp"
#include "amqp_client/connection.hpp"
#include "amqp_client/amqp_error.hpp"

namespace amqp_client
{

namespace rabbitmq_c
{

class Connection : public amqp_client::Connection {
public:
  Connection(
    const amqp_connection_state_t connection,
    const std::shared_ptr<RabbitmqCInterface> & rabbitmq_c_interface
  );

  virtual ~Connection();

  std::shared_ptr<Channel> channel(
    const uint16_t * channel_number = nullptr,
    AMQPError * error = nullptr
  ) override;

  bool close(
    AMQPError * error = nullptr
  ) override;

private:
  AMQP_CLIENT_DISABLE_COPY(Connection)

  amqp_connection_state_t connection_;
  std::shared_ptr<RabbitmqCInterface> rabbitmq_c_interface_;
  amqp_channel_t last_channel_id_;
  std::unordered_set<amqp_channel_t> channel_ids_;

  amqp_channel_t _next_channel_id();
};

}  // namespace rabbitmq_c
}  // namespace amqp_client

#endif  // RMF2_SCHEDULER__AMQP__CONNECTION_RABBIT_MQ_C_HPP_
