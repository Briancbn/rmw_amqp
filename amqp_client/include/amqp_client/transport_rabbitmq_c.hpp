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

#ifndef AMQP_CLIENT__TRANSPORT_RABBITMQ_C_HPP_
#define AMQP_CLIENT__TRANSPORT_RABBITMQ_C_HPP_

#include <memory>

#include "amqp_client/macros.hpp"
#include "amqp_client/transport.hpp"
#include "amqp_client/rabbitmq_c_api.hpp"

namespace amqp_client
{

namespace rabbitmq_c
{

class Transport : public amqp_client::Transport
{
public:
  explicit Transport(std::shared_ptr<RabbitmqCInterface> rabbitmq_c_interface);

  std::shared_ptr<amqp_client::Connection> create_connection(
    const ConnectionInfo::ConstPtr & info,
    AMQPError * error = nullptr
  ) override;

private:
  AMQP_CLIENT_DISABLE_COPY(Transport)

  std::shared_ptr<RabbitmqCInterface> rabbitmq_c_interface_;
};

}  // namespace rabbitmq_c
}  // namespace amqp_client

#endif  // AMQP_CLIENT__TRANSPORT_RABBITMQ_C_HPP_
